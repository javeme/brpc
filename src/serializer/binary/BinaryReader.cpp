#pragma once
#include "stdafx.h"
#include "BinaryReader.h"
#include "CodeUtil.h"

namespace brpc{

const cstring MAP_CLASS_METHOD = "Method";	

BinaryReader::BinaryReader():inputStream(null)
{
}

BinaryReader::~BinaryReader()
{
}


bool BinaryReader::readBool(Boolean& bul)
{
	bul = inputStream->readBoolean();
	return true;
}

bool BinaryReader::readInt(Integer& num)
{
	num.setValue(inputStream->readInt());
	return true;
}

bool BinaryReader::readFloat(Double& num)
{
	num.setValue(inputStream->readDouble());
	return true;
}

bool BinaryReader::readString(TString& str)
{
	str = inputStream->readString();
	return true;
}

bool BinaryReader::readArray(ObjectList& list)
{
	int size = readSize();
	for(int i=0; i<size; i++)
	{	
		Object* val = null;
		if(!readValue(val))
			return false;		
		(void)list.add(val);
	}	
	return true;
}

bool BinaryReader::readObject(ObjectMap& obj)
{
	int size = readSize();
	obj.setClassType(readString());
	for(int i=0; i<size; i++)
	{
		//key		
		String key = readString();	

		//value		
		Object* val = null;
		if(!readValue(val))
			return false;
		
		(void)obj.put(key, val);
	}	
	return true;
}

bool BinaryReader::readValue(Object*& val)
{
	bool success = false;
	byte tag = readTag();
	if(tag == TAG_STR_BEGIN){
		TString* obj = new TString();
		val = obj;
		success = readString(*obj);
	}
	else if(tag == TAG_INT_BEGIN){
		Integer* obj = new Integer();
		val = obj;
		success = readInt(*obj);
	}
	else if(tag == TAG_MAP_BEGIN){
		ObjectMap* obj = new ObjectMap();
		val = obj;
		success = readObject(*obj);
	}
	else if(tag == TAG_ARR_BEGIN){
		ObjectList* obj = new ObjectList();
		val = obj;
		success = readArray(*obj);
	}
	else if(tag == TAG_FLT_BEGIN){
		Double* obj = new Double();
		val = obj;
		success = readFloat(*obj);
	}
	else if(tag == TAG_BOOL_BEGIN){
		Boolean* obj = new Boolean();
		val = obj;
		success = readBool(*obj);
	}
	else if(tag == TAG_NULL){
		success = true;
		val = null;
	}
	else{
		addError("unexpected tag " + (char)tag);
	}

	if (!success) {
		delete val;
		val = null;
	}

	return success;
}

bool BinaryReader::parse(const InputStream& input, ObjectMap*& methodObjMap)
{
	errorList.clear();

	inputStream = const_cast<InputStream*>(&input);
	
	Object* val = null;	
	if(readValue(val) && val != null){
		methodObjMap = dynamic_cast<ObjectMap*>(val);
		if(methodObjMap == null){
			addError(String::format("'%s' is an invalid object",
				val->toString().c_str()));
			delete val;
			return false;
		}
		if(methodObjMap->getClassType() != MAP_CLASS_METHOD){
			addError(String::format("'%s' is not a method",
				methodObjMap->getClassType().c_str()));
			return false;
		}

		return true;
	}
	return false;
}

String BinaryReader::getFormatedErrorMessages() const
{
	StringBuilder sb(errorList.size() * 20);
	for(unsigned int i = 0; i < errorList.size(); i++)
	{
		sb.append(errorList[i]);
	}
	return sb.toString();
}

void BinaryReader::addError( const String& str )
{
	(void)errorList.add(str);
}

void BinaryReader::mustMatched(byte tag)
{
	byte readed = readTag();
	if(readed != tag)
		throw SerializeException(String::format("'%c' not matched tag '%c'",
			readed, tag));
}

int BinaryReader::readSize()
{
	mustMatched(TAG_INT_BEGIN);
	return inputStream->readInt();
}

String BinaryReader::readString()
{
	mustMatched(TAG_STR_BEGIN);
	return inputStream->readString();
}

}