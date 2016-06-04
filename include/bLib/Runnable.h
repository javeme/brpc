#ifndef _Runnable_h_ 
#define _Runnable_h_ 

#include "bluemeiLib.h"
#include "Object.h"

namespace bluemei{


class BLUEMEILIB_API Runnable : public Object
{
public:
	virtual void run()=0;
};


}//end of namespace bluemei
#endif