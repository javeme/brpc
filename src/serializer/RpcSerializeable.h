#pragma once
#include "blib/ByteBuffer.h"
#include "src/rpc/RpcMethod.h"
#include "src/util/RpcException.h"

namespace brpc{

//typedef std::ostream OutputStream;
//typedef std::istream InputStream;
typedef ByteBuffer OutputStream;
typedef ByteBuffer InputStream;

class SerializeException : public RpcException
{
public:
	SerializeException(cstring str):RpcException(str){}
	virtual String toString() const
	{
		return "SerializeException: " + this->m_strLastError;
	}
};

//序列化接口
class RpcSerializeable : public virtual Object
{
	//DECLARE_DCLASS(RpcSerializeable);
public:
	virtual int write(OutputStream& output, const RpcMethod& method,
		const String& encoding="") throw(SerializeException)=0;
	virtual int read(RpcMethod& method, const InputStream& input,
		const String& encoding="") throw(SerializeException)=0;
public:
	inline static String convertEncoding(const String& src,
		const String& encodingFrom, const String& encodingTo);
};

inline String RpcSerializeable::convertEncoding(const String& src,
	const String& encodingFrom, const String& encodingTo)
{
	// TODO: use decode()/encode()
	/*
	String unicode;
	if(!encodingFrom.compare("unicode", false))
		unicode = CodeUtil::decode(src, encodingFrom);
	else
		unicode = src;

	if(!encodingTo.compare("unicode", false))
		return CodeUtil::encode(unicode, encodingTo);
	else
		return unicode;
	*/
	if(encodingTo.compare("gbk", false)){
		if (encodingFrom.compare("utf-8", false)){//utf8-->gbk
			return CodeUtil::utf8ToGbk(src);
		}
	}
	else if(encodingTo.compare("utf-8", false)){
		if (encodingFrom.compare("gbk", false)){//gbk-->utf8
			return CodeUtil::gbkToUtf8(src);
		}
	}

	return src;
}

}//end of namespace brpc