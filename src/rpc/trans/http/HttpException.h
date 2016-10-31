#pragma once
#include "src/util/RpcException.h"


namespace brpc{

class HttpException : public blib::Exception
{
public:
	HttpException(cstring msg);
	virtual String name()const;
};

class HttpParseException : public HttpException
{
public:
	HttpParseException(cstring msg);
	virtual String name()const;
};

class HttpBadRequestException : public HttpException
{
public:
	HttpBadRequestException(cstring msg);
	virtual String name()const;
};

}