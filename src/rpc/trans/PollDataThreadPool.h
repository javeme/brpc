#pragma once
#include "stdafx.h"
#include "RpcSocket.h"


namespace brpc{

class PollDataThreadPool : public Object
{
public:
	PollDataThreadPool(int poolSize=10);
	virtual ~PollDataThreadPool();
public:
	virtual void addSocket(RpcSocket* socket);
	virtual void stopAndWait();
protected:
	ThreadPool pool;
};

}//end of namespace brpc