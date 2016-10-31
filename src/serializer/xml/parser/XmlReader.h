#pragma once
#include "Markup.h"
#include "src/type/ObjectMap.h"
#include "src/type/ObjectList.h"


namespace Xml{

using namespace brpc;

typedef CMarkup XmlParser;

class Reader : public Object
{
public:
	Reader();
	virtual ~Reader();
public:
	virtual bool parse(const String& strValue, ObjectMap*& methodObjMap);
	virtual String getFormatedErrorMessages() const;
protected:
	virtual bool readBool(Boolean& bul);
	virtual bool readInt(Integer& num);
	virtual bool readFloat(Double& num);
	virtual bool readString(TString& str);
	virtual bool readArray(ObjectList& list);
	virtual bool readObject(ObjectMap& obj);
	virtual bool readValue(Object*& val);
protected:
	void addError(const String& str);
protected:
	XmlParser parser;
	ArrayList<String> errorList;
};

}//end of namespace brpc