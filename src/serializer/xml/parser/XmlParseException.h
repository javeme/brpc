#pragma once
#include "RpcSerializeable.h"

namespace bluemei{
	
/*
* Xml解析异常类
* @author 李章梅
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