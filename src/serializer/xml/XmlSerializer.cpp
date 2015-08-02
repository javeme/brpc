#pragma once
#include "stdafx.h"
#include "XmlSerializer.h"
#include "RpcSerializerFactory.h"
#include "ObjectList.h"
#include "ObjectMap.h"

#include "parser/XmlReader.h"


namespace brpc{


const cstring TAG_INT_BEGIN = "<int>";
const cstring TAG_INT_END   = "</int>";
const cstring TAG_FLT_BEGIN = "<float>";
const cstring TAG_FLT_END   = "</float>";
const cstring TAG_STR_BEGIN = "<string>";
const cstring TAG_STR_END   = "</string>";
const cstring TAG_ARR_BEGIN = "<list>";
const cstring TAG_ARR_END   = "</list>";
const cstring TAG_MAP_BEGIN = "<object>";
const cstring TAG_MAP_END   = "</object>";
const cstring TAG_ELE_SEPR  = "";
const cstring TAG_ELE_BEGIN = "<item>";
const cstring TAG_ELE_END   = "</item>";
const cstring TAG_KV_SPL    = " ";
const cstring TAG_KV_BEGIN  = "<pair>";
const cstring TAG_KV_END    = "</pair>";
const cstring TAG_KEY_BEGIN  = "<key>";
const cstring TAG_KEY_END    = "</key>";
const cstring TAG_VAL_BEGIN  = "<value>";
const cstring TAG_VAL_END    = "</value>";
const cstring TAG_BOOL_BEGIN = "<bool>";
const cstring TAG_BOOL_END   = "</bool>";
const cstring TAG_BOOL_TRUE  = "true";
const cstring TAG_BOOL_FALSE = "false";
const cstring TAG_NULL       = "<null/>";

REG_SERIALIZER("text/xml", RpcMethodXmlSerializer)
REG_SERIALIZER("application/xml", RpcMethodXmlSerializer)

Type2XmlSerializer::Type2XmlSerializer()
{
	reset();
}

Type2XmlSerializer::~Type2XmlSerializer()
{
	;
}

void Type2XmlSerializer::visit(Number* v)
{
	checkNullPtr(v);
	if(v->precedence() == PRED_BOOL){ //dynamic_cast<Boolean*>(v)
		append(TAG_BOOL_BEGIN);
		append(v->toString());
		append(TAG_BOOL_END);
	}
	else if(v->precedence() <= PRED_INT){
		append(TAG_INT_BEGIN);
		append(v->toString());
		append(TAG_INT_END);
	}
	else{ //if(v->precedence() <= PRED_DOUBLE)
		append(TAG_FLT_BEGIN);
		append(v->toString());
		append(TAG_FLT_END);
	}
	append(TAG_ELE_SEPR);
}

void Type2XmlSerializer::visit(String* v)
{
	checkNullPtr(v);
	append(TAG_STR_BEGIN);
	append(escapeString(*v));
	append(TAG_STR_END);
	append(TAG_ELE_SEPR);
}

void Type2XmlSerializer::visit(ObjectList* v)
{
	checkNullPtr(v);
	append(TAG_ARR_BEGIN);
	v->visitElements(this);
	removeLastEleseparator();
	append(TAG_ARR_END);
	append(TAG_ELE_SEPR);
}

void Type2XmlSerializer::visit(ObjectMap* v)
{
	checkNullPtr(v);
	//append(TAG_MAP_BEGIN);
	const static String TAG_CLS = String(TAG_MAP_BEGIN).replace(">", 
		" class=\"%s\">");
	String tag = String::format(TAG_CLS, escapeString(v->getClassType()).c_str());
	append(tag);
	v->visitElements(this);
	removeLastEleseparator();
	append(TAG_MAP_END);
	append(TAG_ELE_SEPR);
}

void Type2XmlSerializer::visit(TypeIterator* v)
{
	if(dynamic_cast<ObjectList::Iterator*>(v))
	{
		Object* value = dynamic_cast<ObjectList::Iterator*>(v)->value;
		append(TAG_ELE_BEGIN);
		if(value == null){
			append(TAG_NULL);
			append(TAG_ELE_SEPR);
		}
		else{
			TypeVisitable* visitable=dynamic_cast<TypeVisitable*>(value);
			if(visitable)
				visitable->accept(this);
			else
				visit(&value->toString());
		}
		append(TAG_ELE_END);
	}
	else if(dynamic_cast<ObjectMap::Iterator*>(v))
	{
		ObjectMap::Iterator* itor = dynamic_cast<ObjectMap::Iterator*>(v);
		append(TAG_KV_BEGIN);
		//"age":18
		//key
		append(TAG_KEY_BEGIN);
		append(itor->key);
		append(TAG_KEY_END);
		//":"
		append(TAG_KV_SPL);
		//value
		append(TAG_VAL_BEGIN);
		if(itor->value == null){
			append(TAG_NULL);
			append(TAG_ELE_SEPR);
		}
		else{
			TypeVisitable* visitable=dynamic_cast<TypeVisitable*>(itor->value);
			if(visitable)
				visitable->accept(this);
			else
				visit(&itor->value->toString());
		}
		append(TAG_VAL_END);	
		append(TAG_KV_END);
	}
	else
	{
		;
	}
}

String Type2XmlSerializer::result() const
{
	return strBuilder.toString();
}

void Type2XmlSerializer::reset()
{
	strBuilder.clear();
}

void Type2XmlSerializer::removeLastEleseparator()
{
	unsigned int last = strBuilder.length()-1;
	if (strBuilder.length()>0 
		&& strBuilder.substring(last)==TAG_ELE_SEPR)
	{
		strBuilder.deleteSub(last, last+strlen(TAG_ELE_SEPR));
	}
}

String Type2XmlSerializer::toXml(TypeVisitable* type)
{
	checkNullPtr(type);
	this->reset();
	type->accept(this);
	removeLastEleseparator();
	return result();
}

String Type2XmlSerializer::escapeString(const char *str)
{
	// Not sure how to handle unicode...
	if (strpbrk(str, "<>&\"\' ") == NULL)
		return str;
	// We have to walk value and escape any special characters.
	// Appending to std::string is not efficient, but this should be rare.
	// (Note: forward slashes are *not* rare, but I am not escaping them.)
	unsigned maxsize = strlen(str)*2 + 3; // allescaped+quotes+NULL
	std::string result;
	result.reserve(maxsize); // to avoid lots of mallocs
	for (const char* c=str; *c != 0; ++c)
	{
		switch(*c)
		{
		case '\"':
			result += "&quot;";
			break;
		case '\'':
			result += "&apos;";
			break;
		case '&':
			result += "&amp;";
			break;
		case '<':
			result += "&lt;";
			break;
		case '>':
			result += "&gt;";
			break;
		case ' ':
			//result += "&nbsp;";
			result += " ";
			break;		
		default:
			result += *c;
			break;
		}
	}
	return result;
}


RpcMethodXmlSerializer::RpcMethodXmlSerializer()
{
	;
}

int RpcMethodXmlSerializer::write(OutputStream& output, const RpcMethod& method,
	const String& encoding)
{	
	if (method.status == RpcMethod::STATUS_REQUEST 
		&& method.methodName == "")
	{
		return -1;
	}
	ObjectMap methodObjMap("Method");
	methodObjMap.putValue("version", method.version);
	methodObjMap.putValue("status", method.status);
	methodObjMap.putValue("authToken", method.authToken);
	methodObjMap.putValue("owner", method.owner);
	methodObjMap.putValue("method", method.methodName);
	methodObjMap.putValue("needReturn", method.waitResult);
	methodObjMap.put("args", const_cast<Object*>(method.args));
	if(method.status >= RpcMethod::STATUS_RESPONSE_OK)
		methodObjMap.put("returnValue", method.returnValue);
	Type2XmlSerializer xmlVisiter;
	String result = xmlVisiter.toXml(&methodObjMap);
	//gbk->encoding
	if (encoding.length() > 0 && !encoding.compare("gbk", false)){
		result = convertEncoding(result, "gbk", encoding);
	}
	(void)methodObjMap.remove("args", false);
	(void)methodObjMap.remove("returnValue", false);
	unsigned int count = result.length();
	output.writeBytes((const byte*)result.c_str(), count);
	//printf(">>>>write: %s\n\n", result.c_str());
	return (int)count;
}

int RpcMethodXmlSerializer::read(RpcMethod& method, const InputStream& input,
	const String& encoding)
{
	method.reset();

	String strValue((cstring)input.array(),input.size());
	//encoding->gbk
	if (encoding.length() > 0 && !encoding.compare("gbk", false)){
		strValue = convertEncoding(strValue, encoding, "gbk").c_str();
	}
	//printf(">>>>read: %s\n\n", strValue.c_str());
	/*method.name = "echo";
	method.args = new ObjectList();
	method.autoDel = true;
	method.args->addValue("hi rpc");
	return input.size();*/

	Xml::Reader reader;
	ObjectMap* methodObjMap = null;
	if (reader.parse(strValue, methodObjMap))
	{
		method.setAutoDelArgs(true);
		methodObjMap->getValue("version", method.version);
		methodObjMap->getValue("status", method.status);
		methodObjMap->getValue("authToken", method.authToken);
		methodObjMap->getValue("owner", method.owner);
		methodObjMap->getValue("method", method.methodName);
		method.args = methodObjMap->remove("args", false);
		method.returnValue = methodObjMap->remove("returnValue", false);
		methodObjMap->getValue("needReturn", method.waitResult);
		
		delete methodObjMap;
		methodObjMap = null;

		return int(input.size());
	}
	else
	{
		delete methodObjMap;
		methodObjMap = null;

		String err = reader.getFormatedErrorMessages();
		throwpe(SerializeException(err.c_str()));
	}
	return -1;
}


}//end of namespace bluemei