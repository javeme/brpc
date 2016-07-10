#pragma once
#include "Object.h"
#include "IOCompletionPortImpl.h"

namespace bluemei{


class BLUEMEILIB_API IOCompletionPort : public Object
{
public:
	IOCompletionPort();
	virtual ~IOCompletionPort();
public:
	void registerEvents(int events,socket_t socket){ return impl->registerEvents(events,socket); }
	void unregisterEvents(int events,socket_t socket){ return impl->unregisterEvents(events,socket); }
	void modifyEvents(int events,socket_t socket){ return impl->modifyEvents(events,socket); }

	int waitEvent(IOEvent* events,int maxEvents,int timeout){ return impl->waitEvent(events,maxEvents,timeout); }
	bool cancelWait(){ return impl->cancelWait(); }
	void releaseEventBuffer(IOEvent* e){ return impl->releaseEventBuffer(e); }

	void send( const byte* buf, unsigned int len, socket_t sock ){	return impl->send(buf,len,sock); }


	//bool setOption();
protected:
	void create();
	void close();
protected:
	IOCompletionPortImpl* impl;
};

}//end of namespace bluemei