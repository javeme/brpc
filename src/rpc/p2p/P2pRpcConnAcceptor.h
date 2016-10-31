#pragma once
#include "src/rpc/server/RpcServer.h"
#include "src/rpc/trans/RpcSocket.h"
#include "src/rpc/trans/PollDataThreadPool.h"


namespace brpc{

class P2pRpcConnAcceptor : public RpcConnAcceptor
{
public:
	P2pRpcConnAcceptor(RpcServer* server);
	virtual ~P2pRpcConnAcceptor(){}
public:
	virtual void run();
	virtual void stop();
	virtual void wait();
protected:
	virtual void waitConnection();
	virtual void addConnection(RpcSocket* rpcSocket);
private:
	Url url;
	volatile bool running;
	PollDataThreadPool pool;
};

}//end of namespace brpc