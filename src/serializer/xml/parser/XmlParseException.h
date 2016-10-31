#pragma once
#include "src/serializer/RpcSerializeable.h"

namespace brpc{

/*
* XmlΩ‚Œˆ“Ï≥£¿‡
* @author Javeme
* @create 2012/3/30
*/
class XmlParseException : public SerializeException
{
public:
	XmlParseException(int id,const char* msg):SerializeException(msg),id(id){};
	~XmlParseException(void);

	virtual String toString()const;
protected:
	int id;
};

}