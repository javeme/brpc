#pragma once
#include "blib/ThreadPool.h"
#include "src/rpc/trans/RpcSocket.h"


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