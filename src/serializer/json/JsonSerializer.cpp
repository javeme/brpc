#pragma once
#include "src/serializer/json/JsonSerializer.h"
#include "src/serializer/RpcSerializerFactory.h"
#include "src/type/ObjectList.h"
#include "src/type/ObjectMap.h"

#include "src/serializer/json/jsoncpp/reader.h"
#include <sstream>
#include <iosfwd>
#include <iomanip>


namespace brpc{

const cstring TAG_STR_BEGIN = "\"";
const cstring TAG_STR_END   = "\"";
const cstring TAG_ARR_BEGIN = "[";
const cstring TAG_ARR_END   = "]";
const cstring TAG_MAP_BEGIN = "{";
const cstring TAG_MAP_END   = "}";
const cstring TAG_ELE_SEPR  = ",";
const cstring TAG_KV_SPL    = ":";
const cstring TAG_BOOL_TRUE = "true";
const cstring TAG_BOOL_FALSE= "false";
const cstring TAG_JSON_NULL = "null";

REG_SERIALIZER("text/json", RpcMethodJsonSerializer)
REG_SERIALIZER("application/json", RpcMethodJsonSerializer)

Type2JsonSerializer::Type2JsonSerializer()
{
	reset();
}

Type2JsonSerializer::~Type2JsonSerializer()
{
	;
}

void Type2JsonSerializer::visit(Number* v)
{
	checkNullPtr(v);
	append(v->toString());
	append(TAG_ELE_SEPR);
}

void Type2JsonSerializer::visit(String* v)
{
	checkNullPtr(v);
	/*append(TAG_STR_BEGIN);
	append(*v);
	append(TAG_STR_END);*/
	append(stringToQuotedString(*v));
	append(TAG_ELE_SEPR);
}

void Type2JsonSerializer::visit(ObjectList* v)
{
	checkNullPtr(v);
	append(TAG_ARR_BEGIN);
	v->visitElements(this);
	removeLastEleseparator();
	append(TAG_ARR_END);
	append(TAG_ELE_SEPR);
}

void Type2JsonSerializer::visit(ObjectMap* v)
{
	checkNullPtr(v);
	append(TAG_MAP_BEGIN);
	v->visitElements(this);
	removeLastEleseparator();
	append(TAG_MAP_END);
	append(TAG_ELE_SEPR);
}

void Type2JsonSerializer::visit(TypeIterator* v)
{
	if(dynamic_cast<ObjectList::Iterator*>(v))
	{
		Object* value = dynamic_cast<ObjectList::Iterator*>(v)->value;
		if(value == null){
			append(TAG_JSON_NULL);
			append(TAG_ELE_SEPR);
			return;
		}
		TypeVisitable* visitable=dynamic_cast<TypeVisitable*>(value);
		if(visitable) {
			visitable->accept(this);
		}
		else {
			String vstr = value->toString();
			visit(&vstr);
		}
	}
	else if(dynamic_cast<ObjectMap::Iterator*>(v))
	{
		ObjectMap::Iterator* itor = dynamic_cast<ObjectMap::Iterator*>(v);
		//"age":18
		//key
		append(TAG_STR_BEGIN);
		append(itor->key);
		append(TAG_STR_END);
		//":"
		append(TAG_KV_SPL);
		//value
		if(itor->value == null){
			append(TAG_JSON_NULL);
			append(TAG_ELE_SEPR);
			return;
		}
		TypeVisitable* visitable=dynamic_cast<TypeVisitable*>(itor->value);
		if(visitable) {
			visitable->accept(this);
		}
		else {
			String vstr = itor->value->toString();
			visit(&vstr);
		}
	}
	else
	{
		;
	}
}

String Type2JsonSerializer::result() const
{
	return strBuilder.toString();
}

void Type2JsonSerializer::reset()
{
	strBuilder.clear();
}

void Type2JsonSerializer::removeLastEleseparator()
{
	unsigned int last = strBuilder.length()-1;
	if (strBuilder.length()>0
		&& strBuilder.substring(last)==TAG_ELE_SEPR)
	{
		strBuilder.deleteSub(last, last+strlen(TAG_ELE_SEPR));
	}
}

String Type2JsonSerializer::toJson(TypeVisitable* type)
{
	checkNullPtr(type);
	this->reset();
	type->accept(this);
	removeLastEleseparator();
	return result();
}

bool Type2JsonSerializer::isControlCharacter(char ch)
{
	return ch > 0 && ch <= 0x1F;
}

bool Type2JsonSerializer::containsControlCharacter(const char* str)
{
	while ( *str )
	{
		if ( isControlCharacter( *(str++) ) )
			return true;
	}
	return false;
}

String Type2JsonSerializer::stringToQuotedString(const char *str)
{
	// Not sure how to handle unicode...
	if (strpbrk(str, "\"\\\b\f\n\r\t") == NULL && !containsControlCharacter( str ))
		return std::string("\"") + str + "\"";
	// We have to walk value and escape any special characters.
	// Appending to std::string is not efficient, but this should be rare.
	// (Note: forward slashes are *not* rare, but I am not escaping them.)
	unsigned maxsize = strlen(str)*2 + 3; // allescaped+quotes+NULL
	std::string result;
	result.reserve(maxsize); // to avoid lots of mallocs
	result += "\"";
	for (const char* c=str; *c != 0; ++c)
	{
		switch(*c)
		{
		case '\"':
			result += "\\\"";
			break;
		case '\\':
			result += "\\\\";
			break;
		case '\b':
			result += "\\b";
			break;
		case '\f':
			result += "\\f";
			break;
		case '\n':
			result += "\\n";
			break;
		case '\r':
			result += "\\r";
			break;
		case '\t':
			result += "\\t";
			break;
			//case '/':
			// Even though \/ is considered a legal escape in JSON, a bare
			// slash is also legal, so I see no reason to escape it.
			// (I hope I am not misunderstanding something.
			// blep notes: actually escaping \/ may be useful in javascript to avoid </
			// sequence.
			// Should add a flag to allow this compatibility mode and prevent this
			// sequence from occurring.
		default:
			if ( isControlCharacter( *c ) )
			{
				std::ostringstream oss;
				oss << "\\u" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << static_cast<int>(*c);
				result += oss.str();
			}
			else
			{
				result += *c;
			}
			break;
		}
	}
	result += "\"";
	return result;
}


RpcMethodJsonSerializer::RpcMethodJsonSerializer()
{
	;
}

int RpcMethodJsonSerializer::write(OutputStream& output, const RpcMethod& method,
	const String& encoding) throw(SerializeException)
{
	if (method.status == RpcMethod::STATUS_REQUEST
		&& method.methodName == "")
	{
		return -1;
	}
	ObjectMap methodObjMap("Method");
	method.writeTo(methodObjMap);
	Type2JsonSerializer jsonVisiter;
	String result = jsonVisiter.toJson(&methodObjMap);
	//@TODO(lzm): don't delete this ptrs twice
	(void)methodObjMap.remove("args", false);
	(void)methodObjMap.remove("returnValue", false);
	//gbk->encoding
	if (encoding.length() > 0 && !encoding.compare("gbk", false)){
		result = convertEncoding(result, "gbk", encoding);
	}
	unsigned int count = result.length();
	output.writeBytes((const byte*)result.c_str(), count);
	//printf(">>>>write: %s\n\n", result.c_str());
	return (int)count;
}

int RpcMethodJsonSerializer::read(RpcMethod& method, const InputStream& input,
	const String& encoding) throw(SerializeException)
{
	method.reset();

	std::string strValue((cstring)input.array(),input.size());
	//encoding->gbk
	if (encoding.length() > 0 && !encoding.compare("gbk", false)){
		strValue = convertEncoding(strValue, encoding, "gbk").c_str();
	}

	Json::Reader reader;
	ObjectMap* methodObjMap = null;
	if (reader.parse(strValue, methodObjMap))
	{
		method.reset();
		method.setAutoDelArgs(true);
		method.readFrom(*methodObjMap);
		//@TODO(lzm): don't delete this ptrs twice
		(void)methodObjMap->remove("args", false);
		(void)methodObjMap->remove("returnValue", false);

		delete methodObjMap;
		methodObjMap = null;

		return int(input.size());
	}
	else
	{
		delete methodObjMap;
		methodObjMap = null;

		std::string err = reader.getFormatedErrorMessages();
		err += " when parsing \"" + strValue + "\"\n";
		throwpe(SerializeException(err.c_str()));
	}
	return -1;
}

}//end of namespace brpc
