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
	this->timeout = timeout; //ms
	this->connAcceptor = RpcInvokerFacatory::loadRpcAcceptor(url, this);
}

RpcServer::~RpcServer()
{
	destroy();
	delete this->connAcceptor;
}

void RpcServer::addConnection(RpcConnection* conn)
{
	if (conn == null)
		return;
	BRpcUtil::debug("RpcServer.addConnection: %s\n", conn->toString().c_str());
	this->connList.addToLast(conn);
	conn->setDataHookHandler(this);
}

void RpcServer::removeConnection(RpcConnection* conn)
{
	if (conn == null)
		return;
	BRpcUtil::debug("RpcServer.removeConnection: %s\n", conn->toString().c_str());
	if(this->connList.remove(conn) >= 0)
		delete conn;
}

void RpcServer::start(RpcService* service)
{
	if (this->running)
		return;

	checkNullPtr(service);
	this->dispatcher = service;
	this->dispatcher->setEventHandler(this);

	checkNullPtr(this->connAcceptor);
	this->connAcceptor->start();

	this->running = true;
}

void RpcServer::wait()
{
	checkNullPtr(this->connAcceptor);
	this->connAcceptor->wait();

	destroy();
	this->running = false;
}

void RpcServer::stop()
{
	this->running = false;
	//wait for stop
	this->wait();
}

void RpcServer::destroy()
{
	auto itor = this->connList.iterator();
	while (itor->hasNext())
	{
		RpcConnection* conn = itor->next();
		delete conn;
	}
	this->connList.clear();
}

void RpcServer::onEvent(cstring event, Object* sender, const ObjectList& args)
{
	auto itor = this->connList.iterator();
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
