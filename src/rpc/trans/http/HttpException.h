#pragma once
#include "Exception.h"

namespace bluemei{

class HttpException : public Exception
{
public:
	HttpException(cstring msg);
	virtual String toString()const;
};

class HttpParseException : public HttpException
{
public:
	HttpParseException(cstring msg);
	virtual String toString()const;
};

}