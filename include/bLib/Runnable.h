#ifndef _Runnable_h_
#define _Runnable_h_

#include "bluemeiLib.h"
#include "Object.h"

namespace blib{


class BLUEMEILIB_API Runnable : public Object
{
public:
	virtual void run()=0;
};


}//end of namespace blib
#endif