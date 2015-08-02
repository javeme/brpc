#pragma once
#include "stdafx.h"
#include "JmtpException.h"

namespace brpc{

JmtpException::JmtpException(cstring msg) : Exception(msg)
{
	;
}

String JmtpException::name() const
{
	return "JmtpException";
}

}