#ifndef IOException_H_H
#define IOException_H_H

#include "bluemeiLib.h"
#include "Exception.h"

namespace bluemei{

class BLUEMEILIB_API IOException:public Exception
{
public:
	IOException();
	IOException(String msg);
	virtual String name() const;
};

}//namespace bluemei
#endif