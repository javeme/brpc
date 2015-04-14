
#ifndef _SocketException_h_
#define _SocketException_h_

#include "Exception.h"

namespace bluemei{


class BLUEMEILIB_API SocketException:public Exception
{
public:
	SocketException(int nError=0);
	SocketException(String strError);
	SocketException(int nError,String strError);
	virtual ~SocketException();
public:
	virtual String name()const;
	virtual int getError();
	virtual void printErrorMsg();
	virtual String toString()const;
protected:
	int m_nError;
};

}//end of namespace bluemei
#endif