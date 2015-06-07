#pragma once
#include "stdafx.h"
#include "LinkedList.h"
#include "FuncDispatcher.h"
#include "RpcSerializeable.h"
#include "AuthChecker.h"
#include "RpcService.h"
#include "RpcDataHookHandler.h"

namespace bluemei{

class RpcConnection;
class RpcConnAcceptor;

#define DFT_TIMEOUT (1000 * 20) //20s

class RpcServer : public RpcEventHandler, public RpcDataHookHandler
{
public:
	RpcServer(cstring url, AuthChecker* authChecker,
		cstring serializerType="text/json", unsigned int timeout=DFT_TIMEOUT);
	virtual ~RpcServer();
public:
	virtual void start(RpcService* service);
	virtual void wait();
	virtual void stop();

public:
	String getUrl() const { return url; }
	RpcService* getDispatcher() const { return dispatcher; }
	AuthChecker* getAuthChecker() const { return authChecker; }
	String getSerializerType() const { return serializerType; }

	unsigned int getTimeout() const { return timeout; }
	void setTimeout(unsigned int val) { timeout = val; }

	void addConnection(RpcConnection* conn);
	void removeConnection(RpcConnection* conn);
public:
	virtual void onEvent(cstring event, Object* sender, const ObjectList& args);

	virtual void onReceived(cstring name,const DataPackage& data,long time);
	virtual void onSent(cstring name,const DataPackage& data,long time);
	virtual void onError(cstring name,cstring err,RpcReceiveListener* listener);
protected:
	void destroy();
private:
	String url;
	RpcService* dispatcher;
	AuthChecker* authChecker;
	String serializerType;
	unsigned int timeout;
private:
	RpcConnAcceptor* connAcceptor;
private:
	LinkedList<RpcConnection*> connList;
	volatile bool running;
};

class RpcConnAcceptor : public Thread
{
public:
	RpcConnAcceptor(RpcServer* server) : server(server){}
	virtual ~RpcConnAcceptor(){}
protected:
	RpcServer* server;
};

}//end of namespace bluemei