#pragma once
#include "stdafx.h"
#include "RpcSocket.h"
#include "ThreadPool.h"

namespace bluemei{

class PollDataThreadPool : public Object
{
public:
	PollDataThreadPool(int poolSize=10);
	virtual ~PollDataThreadPool();
public:
	void addSocket(RpcSocket* socket);
protected:
	ThreadPool pool;
};

}//end of namespace bluemei