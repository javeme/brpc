#pragma once
#include "stdafx.h"
#include "P2pRpcServer.h"
#include "P2pRpcClient.h"
#include "RpcSocketManager.h"
#include "Log.h"

namespace bluemei{


class P2pRpcClientForServer : public P2pRpcClient
{
public:
	P2pRpcClientForServer(cstring url, RpcService* dispatcher, 
		AuthChecker* authChecker, cstring serializerType)
		: P2pRpcClient(url, dispatcher, authChecker, serializerType)
	{
		;
	}
	virtual void waitForConnection()
	{
		if (rpcSocket == null){
			rpcSocket = RpcSocketManager::getRpcSocket(protocol);
			if (rpcSocket == null)
				throwpe(RpcException("invalid url: protocol " + protocol));
			rpcSocket->setReceiveListener(this);
		}
		if (!rpcSocket->isAlive()){
			HashMap<String,String> paras;
			paras.put("server", "true");
			paras.put("port", RPC_P2P_PORT);
			paras.put("timeout", String() + timeout);
			rpcSocket->connect(paras);
		}
	}
protected:
	virtual void checkConnected() 
	{
		if (!rpcSocket->isAlive()){
			throwpe(RpcException("disconnected with peer"));
		}
	}
};


P2pRpcServer::P2pRpcServer(cstring url, 
	AuthChecker* authChecker, cstring serializerType) 
	: url(url), authChecker(authChecker), serializerType(serializerType)
{
	running = false;
}

P2pRpcServer::~P2pRpcServer()
{
	destroy();
}

void P2pRpcServer::run(RpcService* service)
{
	Log* logger = Log::getLogger();
	checkNullPtr(service);
	service->setEventHandler(this);

	running = true;
	while (running)
	{
		P2pRpcClientForServer* client = new P2pRpcClientForServer(url, 
			service, authChecker, serializerType);
		logger->debug("wait for connection @" + url);
		client->waitForConnection();//@Todo: need wait continuous
		logger->debug("connection accepted");
		clientList.addToLast(client);
	}
	destroy();
}

void P2pRpcServer::stop()
{
	running = false;
	//wait for stop
	;
}

void P2pRpcServer::destroy()
{
	auto itor = clientList.iterator();
	while (itor->hasNext())
	{
		P2pRpcClient* client = itor->next();
		delete client;
	}
	clientList.releaseIterator(itor);
	clientList.clear();
}

void P2pRpcServer::onEvent(cstring event, Object* sender, const ObjectList& args)
{
	auto itor = clientList.iterator();
	while (itor->hasNext())
	{
		P2pRpcClient* client = itor->next();
		client->notifyEvent(event, sender, args);
	}
	clientList.releaseIterator(itor);
}

}//end of namespace bluemei


