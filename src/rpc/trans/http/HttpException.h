#pragma once
#include "Exception.h"

namespace bluemei{

class HttpException : public Exception
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