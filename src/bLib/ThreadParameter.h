#ifndef _ThreadParameter_h_
#define _ThreadParameter_h_

#include "bluemeiLib.h"
#include "Runnable.h"

namespace bluemei{

class BLUEMEILIB_API ThreadParameter
{
public:
	Runnable* pObject;
	void* pUserParameter;
};


}//end of namespace bluemei
#endif