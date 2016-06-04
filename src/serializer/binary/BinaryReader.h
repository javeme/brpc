#pragma once
#include "ObjectMap.h"
#include "ObjectList.h"
#include "RpcSerializeable.h"

namespace brpc{
	
const byte TAG_INT_BEGIN = 'i';
const byte TAG_FLT_BEGIN = 'f';
const byte TAG_STR_BEGIN = 's';
const byte TAG_ARR_BEGIN = 'a';
const byte TAG_MAP_BEGIN = 'm';
const byte TAG_BOOL_BEGIN= 'b';
const byte TAG_BOOL_TRUE = 't';
const byte TAG_BOOL_FALSE= 'f';
const byte TAG_NULL		 = 'n';

class BinaryReader : public Object
{
public:
	BinaryReader();
	virtual ~BinaryReader();
public:
	virtual bool parse(const InputStream& input, ObjectMap*& methodObjMap);
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

	virtual byte readTag(){ return inputStream->readByte(); }
	virtual void mustMatched(byte tag);

	virtual int readSize();
	virtual String readString();
protected:
	InputStream* inputStream;
	ArrayList<String> errorList;
};

}//end of namespace brpc