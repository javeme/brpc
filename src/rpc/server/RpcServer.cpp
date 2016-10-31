#pragma once
#include "blib/Log.h"
#include "src/rpc/server/RpcServer.h"
#include "src/rpc/trans/RpcSocketFactory.h"
#include "src/rpc/RpcConnection.h"
#include "src/rpc/RpcInvokerFacatory.h"


namespace brpc{

RpcServer::RpcServer(cstring url, AuthChecker* authChecker,
	cstring serializerType, unsigned int timeout, cstring name)
	: url(url), authChecker(authChecker), dispatcher(null),
	  serializerType(serializerType), name(name)
{
	this->running = false;
	this->timeout = timeout;//ms
	this->connAcceptor = RpcInvokerFacatory::loadRpcAcceptor(url, this);
}

RpcServer::~RpcServer()
{
	destroy();
	delete connAcceptor;
}

void RpcServer::addConnection(RpcConnection* conn)
{
	if (conn == null)
		return;
	BRpcUtil::debug("RpcServer.addConnection: %s\n", conn->toString().c_str());
	connList.addToLast(conn);
	conn->setDataHookHandler(this);
}

void RpcServer::removeConnection(RpcConnection* conn)
{
	if (conn == null)
		return;
	BRpcUtil::debug("RpcServer.removeConnection: %s\n", conn->toString().c_str());
	if(connList.remove(conn) >= 0)
		delete conn;
}

void RpcServer::start(RpcService* service)
{
	if (running)
		return;

	checkNullPtr(service);
	dispatcher = service;
	dispatcher->setEventHandler(this);

	checkNullPtr(connAcceptor);
	connAcceptor->start();

	running = true;
}

void RpcServer::wait()
{
	checkNullPtr(connAcceptor);
	connAcceptor->wait();

	destroy();
	running = false;
}

void RpcServer::stop()
{
	running = false;
	//wait for stop
	this->wait();
}

void RpcServer::destroy()
{
	auto itor = connList.iterator();
	while (itor->hasNext())
	{
		RpcConnection* conn = itor->next();
		delete conn;
	}
	connList.clear();
}

void RpcServer::onEvent(cstring event, Object* sender, const ObjectList& args)
{
	auto itor = connList.iterator();
	while (itor->hasNext())
	{
		RpcConnection* conn = itor->next();
		if(conn->checkConnected()) {
			BRpcUtil::debug("RpcConnection.onEvent: %s(to: %s)\n",
							event, conn->toString().c_str());
			conn->notifyEvent(event, sender, args);
		}
	}
}

//hook the data and log it
void RpcServer::onReceived(cstring name,const DataPackage& data,long time)
{
	String content((cstring)data.body.array(), data.body.size());
	BRpcUtil::debug("====received: %s\n", content.c_str());
	String msg = String::format(
		"<<<< received from %s: headers<%s>, body<%s>",
		name, data.headers.toString().c_str(),
		content.c_str());
	Log::getLogger()->debug(msg);
}

void RpcServer::onSent(cstring name,const DataPackage& data,long time)
{
	String content((cstring)data.body.array(), data.body.size());
	BRpcUtil::debug("====sended: %s\n", content.c_str());
	String msg = String::format(
		">>>> sended to %s: headers<%s>, body<%s>",
		name, data.headers.toString().c_str(),
		content.c_str());
	Log::getLogger()->debug(msg);
}

void RpcServer::onError(cstring name,cstring err,RpcReceiveListener* listener)
{
	String msg = String::format(
		"connection on %s: %s",
		name, err);
	BRpcUtil::debug("====%s\n", msg.c_str());
	Log::getLogger()->debug(msg);

	if(streq(HOOK_ERR_RECV_STOPED, err) ||
	   streq(HOOK_ERR_CLOSED, err))
	{
		removeConnection(dynamic_cast<RpcConnection*>(listener));
	}
}

}//end of namespace brpc