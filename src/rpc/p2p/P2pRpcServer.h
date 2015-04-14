#pragma once
#include "stdafx.h"
#include "RpcServer.h"
#include "LinkedList.h"
#include "FuncDispatcher.h"
#include "RpcSerializeable.h"
#include "AuthChecker.h"
#include "RpcService.h"

namespace bluemei{

class P2pRpcClient;

class P2pRpcServer : public RpcServer, public RpcEventHandler
{
public:
	P2pRpcServer(cstring url, AuthChecker* authChecker, 
		cstring serializerType="text/json");
	virtual ~P2pRpcServer();
public:
	virtual void run(RpcService* service);
	virtual void stop();

public:
	virtual void onEvent(cstring event, Object* sender, const ObjectList& args);
protected:
	void destroy();
private:
	String url;
	FuncDispatcher* dispatcher;
	AuthChecker* authChecker;
	String serializerType;
private:
	LinkedList<P2pRpcClient*> clientList;
	volatile bool running;
};

}//end of namespace bluemei