#pragma once
#ifndef _ThreadException_h_
#define _ThreadException_h_

#include "Exception.h"

namespace blib{

class BLUEMEILIB_API ThreadException : public Exception
{
public:
	ThreadException(int error=0);
	ThreadException(cstring strError,int error=0);
	~ThreadException(void);

public:
	virtual String name() const;
	virtual String getMessage() const;
	virtual String toString() const;
private:
	int m_nError;
};

}//end of namespace blib
#endif