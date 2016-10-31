#pragma once
#include "src/rpc/trans/jmtp/JmtpException.h"


namespace brpc{

JmtpException::JmtpException(cstring msg) : Exception(msg)
{
}

String JmtpException::name() const
{
	return "JmtpException";
}

}