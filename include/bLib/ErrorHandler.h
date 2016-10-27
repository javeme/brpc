#pragma once
#include "Object.h"
#include "Exception.h"

namespace bluemei{

class BLUEMEILIB_API IErrorHandler : public Object
{
public:
	virtual bool handle(Exception& e)=0;
};

class BLUEMEILIB_API ErrorHandler : public Object
{
public:
	static void handle(Exception& e);
	static void setHandler(IErrorHandler* handler);
protected:
	static void handleErrorOfHandler(const Exception& e, const Exception&);
protected:
	static IErrorHandler* s_errorHandler;
};

}//end of namespace bluemei