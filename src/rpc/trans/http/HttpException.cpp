#pragma once
#include "stdafx.h"
#include "HttpException.h"

namespace bluemei{

HttpException::HttpException(cstring msg):Exception(msg)
{
	;
}

String HttpException::name()const
{
	return "HttpException";
}


HttpParseException::HttpParseException(cstring msg):HttpException(msg)
{
	;
}

String HttpParseException::name()const
{
	return "HttpParseException";
}


HttpBadRequestException::HttpBadRequestException(cstring msg):HttpException(msg)
{
	;
}

String HttpBadRequestException::name() const
{
	return "HttpBadRequestException";
}

}