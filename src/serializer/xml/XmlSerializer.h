#pragma once
#include "RpcSerializeable.h"
#include "TypeVisitor.h"


namespace brpc{

class Type2XmlSerializer : public TypeVisiter
{
public:
	Type2XmlSerializer();
	virtual ~Type2XmlSerializer();
public:
	virtual String toXml(TypeVisitable* type);
public:
	virtual void visit(Number* v);
	virtual void visit(String* v);
	virtual void visit(ObjectList* v);
	virtual void visit(ObjectMap* v);
	virtual void visit(TypeIterator* v);
public:
	virtual String result() const;
	virtual void reset();
protected:
	void append(const String& s) { strBuilder.append(s); }
	void removeLastEleseparator();
public:
	static String escapeString(const char *str);
private:
	StringBuilder strBuilder;
};

//序列化接口
class RpcMethodXmlSerializer : public RpcSerializeable
{
public:
	DECLARE_DCLASS(RpcMethodXmlSerializer);
	RpcMethodXmlSerializer();
public:
	virtual int write(OutputStream& output, const RpcMethod& methodconst,
		const String& encoding="");
	virtual int read(RpcMethod& method, const InputStream& input,
		const String& encoding="");
};

}//end of namespace brpc