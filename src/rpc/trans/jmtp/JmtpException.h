#pragma once
#include "blib.h"


namespace brpc{

class JmtpException : public Exception
{
public:
	JmtpException(cstring msg);
	virtual String name()const;
};

}