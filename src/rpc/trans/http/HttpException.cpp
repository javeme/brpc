#pragma once
#include "src/rpc/trans/http/HttpException.h"


namespace brpc{

HttpException::HttpException(cstring msg) : RpcException(msg)
{
	;
}

String HttpException::name()const
{
	return "HttpException";
}


HttpParseException::HttpParseException(cstring msg) : HttpException(msg)
{
	;
}

String HttpParseException::name()const
{
	return "HttpParseException";
}


HttpBadRequestException::HttpBadRequestException(cstring msg)
	: HttpException(msg)
{
	;
}

String HttpBadRequestException::name() const
{
	return "HttpBadRequestException";
}

}
