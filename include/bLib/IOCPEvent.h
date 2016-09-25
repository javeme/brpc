#pragma once
#include "Object.h"
#include "IOCompletionPortImpl.h"


namespace bluemei{

class BLUEMEILIB_API IOCPEventHandle : public Object
{
public:
	virtual bool onAccepted(IOEvent& ev)=0;
	virtual bool onWrite(IOEvent& ev)=0;
	virtual bool onWriteFinish(IOEvent& ev)=0;
	virtual bool onRead(IOEvent& ev)=0;
	virtual bool onReadFinish(IOEvent& ev)=0;
	virtual bool onError(IOEvent& ev)=0;
	virtual bool onClosed(IOEvent& ev)=0;
	virtual bool onException(Exception& e)=0;
};

}//end of namespace bluemei
