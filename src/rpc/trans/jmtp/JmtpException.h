#pragma once
#include "src/util/RpcException.h"


namespace brpc{

class JmtpException : public blib::Exception
{
public:
	JmtpException(cstring msg);
	virtual String name()const;
};

}