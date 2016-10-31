#pragma once
#include "src/serializer/binary/BinarySerializer.h"
#include "src/serializer/binary/BinaryReader.h"
#include "src/serializer/RpcSerializerFactory.h"
#include "src/type/ObjectList.h"
#include "src/type/ObjectMap.h"


namespace brpc{

REG_SERIALIZER("application/brpc.bin", RpcMethodBinarySerializer)

Type2BinarySerializer::Type2BinarySerializer(OutputStream& output)
	:outputStream(output)
{
	//reset();
}

Type2BinarySerializer::~Type2BinarySerializer()
{
	;
}

void Type2BinarySerializer::visit(Number* v)
{
	checkNullPtr(v);
	if(v->precedence() == PRED_BOOL){ //dynamic_cast<Boolean*>(v)
		append(TAG_BOOL_BEGIN);
		append(v->toInt() != 0);
	}
	else if(v->precedence() <= PRED_INT){
		append(TAG_INT_BEGIN);
		append(v->toInt());
	}
	else{ //if(v->precedence() <= PRED_DOUBLE)
		append(TAG_FLT_BEGIN);
		append(v->toDouble());
	}
}

void Type2BinarySerializer::visit(String* v)
{
	checkNullPtr(v);
	append(TAG_STR_BEGIN);
	append(*v);
}

void Type2BinarySerializer::visit(ObjectList* v)
{
	checkNullPtr(v);
	append(TAG_ARR_BEGIN);
	//size
	append(TAG_INT_BEGIN);
	append(v->size());

	v->visitElements(this);
}

void Type2BinarySerializer::visit(ObjectMap* v)
{
	checkNullPtr(v);
	append(TAG_MAP_BEGIN);
	//size
	append(TAG_INT_BEGIN);
	append(v->size());
	//class
	visit(&v->getClassType());

	v->visitElements(this);
}

void Type2BinarySerializer::visit(TypeIterator* v)
{
	if(dynamic_cast<ObjectList::Iterator*>(v))
	{
		Object* value = dynamic_cast<ObjectList::Iterator*>(v)->value;
		if(value == null){
			append(TAG_NULL);
		}
		else{
			TypeVisitable* visitable=dynamic_cast<TypeVisitable*>(value);
			if(visitable)
				visitable->accept(this);
			else
				visit(&value->toString());
		}
	}
	else if(dynamic_cast<ObjectMap::Iterator*>(v))
	{
		ObjectMap::Iterator* itor = dynamic_cast<ObjectMap::Iterator*>(v);

		//key
		append(TAG_STR_BEGIN);
		append(itor->key);
		//value
		if(itor->value == null){
			append(TAG_NULL);
		}
		else{
			TypeVisitable* visitable=dynamic_cast<TypeVisitable*>(itor->value);
			if(visitable)
				visitable->accept(this);
			else
				visit(&itor->value->toString());
		}
	}
	else
	{
		;
	}
}

OutputStream& Type2BinarySerializer::result()
{
	return outputStream;
}

void Type2BinarySerializer::reset()
{
	outputStream.clear();
}

unsigned int Type2BinarySerializer::toBinary(TypeVisitable* type)
{
	checkNullPtr(type);
	unsigned int old = outputStream.size();
	//this->reset();
	type->accept(this);
	return outputStream.size() - old;
}


RpcMethodBinarySerializer::RpcMethodBinarySerializer()
{
	;
}

int RpcMethodBinarySerializer::write(OutputStream& output, const RpcMethod& method,
	const String& encoding)
{
	if (method.status == RpcMethod::STATUS_REQUEST
		&& method.methodName == "")
	{
		return -1;
	}
	ObjectMap methodObjMap("Method");
	method.writeTo(methodObjMap);
	Type2BinarySerializer binVisiter(output);
	unsigned int count = binVisiter.toBinary(&methodObjMap);
	//@TODO(lzm): don't delete this ptrs twice
	(void)methodObjMap.remove("args", false);
	(void)methodObjMap.remove("returnValue", false);
	//printf(">>>>write: %s\n\n", result.c_str());
	return (int)count;
}

int RpcMethodBinarySerializer::read(RpcMethod& method, const InputStream& input,
	const String& encoding)
{
	method.reset();

	//String strValue((cstring)input.array(),input.size());
	//printf(">>>>read: %s\n\n", strValue.c_str());

	BinaryReader reader;
	ObjectMap* methodObjMap = null;
	if (reader.parse(input, methodObjMap))//待加入异常处理!!!
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

		String err = reader.getFormatedErrorMessages();
		throwpe(SerializeException(err.c_str()));
	}
	return -1;
}

}//end of namespace brpc