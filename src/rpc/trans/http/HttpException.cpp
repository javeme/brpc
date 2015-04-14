#pragma once
#include "stdafx.h"
#include "HttpException.h"

namespace bluemei{

HttpException::HttpException(cstring msg):Exception(msg)
{
	;
}

String HttpException::toString()const
{
	return "HttpException:"+m_strLastError;
}


HttpParseException::HttpParseException(cstring msg):HttpException(msg)
{
	;
}

String HttpParseException::toString()const
{
	return "HttpParseException:"+m_strLastError;
}

}