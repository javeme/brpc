#ifndef IOException_H_H
#define IOException_H_H

#include "bluemeiLib.h"
#include "Exception.h"

namespace blib{

class BLUEMEILIB_API IOException:public Exception
{
public:
	IOException();
	IOException(String msg);
	virtual String name() const;
};

}//namespace blib
#endif