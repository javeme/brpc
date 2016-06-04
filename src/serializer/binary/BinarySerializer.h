#pragma once
#include "RpcSerializeable.h"
#include "TypeVisitor.h"

namespace brpc{


class Type2BinarySerializer : public TypeVisiter
{
public:
	Type2BinarySerializer(OutputStream& output);
	virtual ~Type2BinarySerializer();
public:
	virtual unsigned int toBinary(TypeVisitable* type);
public:
	virtual void visit(Number* v);
	virtual void visit(String* v);
	virtual void visit(ObjectList* v);
	virtual void visit(ObjectMap* v);
	virtual void visit(TypeIterator* v);
public:
	virtual OutputStream& result();
	virtual void reset();
protected:
	void append(cstring s) { (void)outputStream.writeString(s); }
	void append(const String& s) { (void)outputStream.writeString(s); }
	void append(double v) { (void)outputStream.writeDouble(v); }
	void append(int v) { (void)outputStream.writeInt(v); }
	void append(unsigned int v) { (void)outputStream.writeInt((int)v); }
	void append(byte v) { (void)outputStream.writeByte(v); }
	void append(bool v) { (void)outputStream.writeBoolean(v); }
private:
	OutputStream& outputStream;
};


//序列化接口
class RpcMethodBinarySerializer : public RpcSerializeable
{
public:
	DECLARE_DCLASS(RpcMethodBinarySerializer);
	RpcMethodBinarySerializer();
public:
	virtual int write(OutputStream& output, const RpcMethod& method,
		const String& encoding="");
	virtual int read(RpcMethod& method, const InputStream& input,
		const String& encoding="");
};


}//end of namespace brpc