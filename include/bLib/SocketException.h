
#ifndef _SocketException_h_
#define _SocketException_h_

#include "Exception.h"

namespace blib{


class BLUEMEILIB_API SocketException : public Exception
{
public:
	SocketException(int nError=0);
	SocketException(const String& strError);
	SocketException(int nError,const String& strError);
	virtual ~SocketException();
public:
	virtual String name() const;
	virtual int getError();
	virtual void printErrorMsg();
	virtual String toString() const;
protected:
	int m_nError;
};

class BLUEMEILIB_API SocketClosedException : public SocketException
{
public:
	SocketClosedException(const String& strError) : SocketException(strError){}
	virtual String name() const { return "SocketClosedException"; }
};

}//end of namespace blib
#endif