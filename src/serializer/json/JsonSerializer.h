#pragma once
#include "src/serializer/RpcSerializeable.h"
#include "src/type/TypeVisitor.h"


namespace brpc{

class Type2JsonSerializer : public TypeVisiter
{
public:
	Type2JsonSerializer();
	virtual ~Type2JsonSerializer();
public:
	virtual String toJson(TypeVisitable* type);
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
	static bool isControlCharacter(char ch);
	static bool containsControlCharacter( const char* str );
	static String stringToQuotedString(const char *str);
private:
	StringBuilder strBuilder;
};

//序列化接口
class RpcMethodJsonSerializer : public RpcSerializeable
{
public:
	DECLARE_DCLASS(RpcMethodJsonSerializer);
	RpcMethodJsonSerializer();
public:
	virtual int write(OutputStream& output, const RpcMethod& method,
		const String& encoding="");
	virtual int read(RpcMethod& method, const InputStream& input,
		const String& encoding="");
};

}//end of namespace brpc