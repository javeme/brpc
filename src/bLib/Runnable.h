#ifndef _Runnable_h_ 
#define _Runnable_h_ 

#include "bluemeiLib.h"
#include "Object.h"

namespace bluemei{

class ThreadParameter;

class BLUEMEILIB_API Runnable : public Object
{
public:
	virtual void run(ThreadParameter*)=0;
};


}//end of namespace bluemei
#endif