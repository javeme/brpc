#pragma once
#include "stdafx.h"
#include "XmlReader.h"
#include "CodeUtil.h"

namespace Xml{

using namespace bluemei;

const cstring TAG_BOOL= "bool";
const cstring TAG_INT = "int";
const cstring TAG_FLT = "float";
const cstring TAG_STR = "string";
const cstring TAG_ARR = "list";
const cstring TAG_MAP = "object";
const cstring TAG_ELE = "item";
const cstring TAG_KV  = "pair";
const cstring TAG_KEY = "key";
const cstring TAG_VAL = "value";
const cstring TAG_TRUE  = "true";
const cstring TAG_FALSE = "false";
const cstring TAG_NULL  = "null";

const cstring MAP_CLASS_METHOD = "Method";

Reader::Reader()
{
}

Reader::~Reader()
{
}

bool Reader::readBool(Boolean& bul)
{
	std::string str = parser.GetData();
	//checkBool(str);
	bul = str == TAG_TRUE;
	return true;
}

bool Reader::readInt(Integer& num)
{
	std::string str = parser.GetData();
	//checkInt(str);
	num.setValue(CodeUtil::str2Int(str.c_str()));
	return true;
}

bool Reader::readFloat(Double& num)
{
	std::string str = parser.GetData();
	//checkDouble(str);
	num.setValue(CodeUtil::str2Float(str.c_str()));
	return true;
}

bool Reader::readString(TString& str)
{
	str = parser.GetData().c_str();
	return true;
}

bool Reader::readArray(ObjectList& list)
{
	while(parser.FindChildElem(TAG_ELE))
	{
		parser.IntoElem();//into item

		//type
		if(!parser.FindChildElem()){
			addError("expected type");
			return false;
		}
		parser.IntoElem();//into type
		Object* val = null;
		if(!readValue(val))
			return false;
		parser.OutOfElem();//outof type

		(void)list.add(val);

		parser.OutOfElem();//outof item
	}	
	return true;
}

bool Reader::readObject(ObjectMap& obj)
{
	std::string cls = parser.GetAttrib("class");
	obj.setClassType(cls.c_str());

	while(parser.FindChildElem(TAG_KV))
	{
		parser.IntoElem();//into pair

		//key
		if(!parser.FindChildElem(TAG_KEY)){
			addError("expected key");
			return false;
		}
		std::string key = parser.GetChildData();	

		//value
		parser.ResetChildPos();
		if(!parser.FindChildElem(TAG_VAL)){
			addError("expected value");
			return false;
		}
		parser.IntoElem();//into value
		if(!parser.FindChildElem()){//type such as int/string
			addError("expected type");
			return false;
		}
		parser.IntoElem();//into type
		Object* val = null;
		if(!readValue(val))
			return false;
		parser.OutOfElem();//outof type
		parser.OutOfElem();//outof value

		(void)obj.put(key, val);

		parser.OutOfElem();//outof pair
	}	
	return true;
}

bool Reader::readValue(Object*& val)
{
	bool success = false;
	std::string tag = parser.GetTagName();
	if(tag == TAG_STR){
		TString* obj = new TString();
		success = readString(*obj);
		val = obj;
	}
	else if(tag == TAG_INT){
		Integer* obj = new Integer();
		success = readInt(*obj);
		val = obj;
	}
	else if(tag == TAG_MAP){
		ObjectMap* obj = new ObjectMap();
		success = readObject(*obj);
		val = obj;
	}
	else if(tag == TAG_ARR){
		ObjectList* obj = new ObjectList();
		success = readArray(*obj);
		val = obj;
	}
	else if(tag == TAG_FLT){
		Double* obj = new Double();
		success = readFloat(*obj);
		val = obj;
	}
	else if(tag == TAG_BOOL){
		Boolean* obj = new Boolean();
		success = readBool(*obj);
		val = obj;
	}
	else if(tag == TAG_NULL){
		success = true;
		val = null;
	}
	else{
		addError("unexpected tag " + tag);
	}

	if (!success) {
		delete val;
		val = null;
	}

	return success;
}

bool Reader::parse(const String& strValue, ObjectMap*& methodObjMap)
{
	errorList.clear();

	parser.SetDocFlags(CMarkup::MDF_IGNORECASE);
	if(!parser.Parse(strValue))
	{
		return false;
	}
	else if(parser.FindElem())
	{
		std::string tag = parser.GetTagName();
		std::string cls=parser.GetAttrib("class");
		if(tag == TAG_MAP && cls == MAP_CLASS_METHOD){
			methodObjMap = new ObjectMap();
			if(!readObject(*methodObjMap)){
				//delete methodObjMap;
				//methodObjMap = null;
				return false;
			}
			return true;
		}
		else
		{
			addError(String::format("'%s<%s>' is an invalid object",
				tag.c_str(), cls.c_str()));
			return false;
		}
	}
	else
	{
		addError("not found any element");
		return false;
	}
}

String Reader::getFormatedErrorMessages() const
{
	return errorList.toString();
}

void Reader::addError( const String& str )
{
	(void)errorList.add(str);
}


}