#pragma once
#include "Object.h"

#if defined(WIN32)
	#include "IOCompletionPortImplWin32.h"
#else
	#include "IOCompletionPortImplLinux.h"
#endif

namespace blib{

/*
class IOCompletionPortImpl : public Object
{
public:
	virtual void registerEvents(int events,socket_t socket)=0;
	virtual void unregisterEvents(int events,socket_t socket)=0;
	virtual void modifyEvents(int events,socket_t socket)=0;

	virtual int waitEvent(IOEvent* events,int maxEvents,int timeout)=0;
	virtual bool cancelWait()=0;
public:
	virtual void create()=0;
	virtual void close()=0;
};*/

}//end of namespace blib