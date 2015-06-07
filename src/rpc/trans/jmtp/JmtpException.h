#pragma once
#include "Exception.h"

namespace bluemei{
	
class JmtpException : public Exception
{
public:
	JmtpException(cstring msg);
	virtual String name()const;
};

}