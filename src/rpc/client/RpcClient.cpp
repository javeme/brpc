#pragma once
#include "stdafx.h"
#include "RpcClient.h"
#include "RpcInvokerFacatory.h"

namespace brpc{


RpcClient::RpcClient(cstring url, RpcService* dispatcher, AuthChecker* authChecker,
	cstring serializerType, unsigned int timeout)
{
	this->rpcInvoker = RpcInvokerFacatory::loadRpcInvoker(url, 
		dispatcher, authChecker, serializerType, timeout);
	checkNullPtr(this->rpcInvoker);
}

RpcClient::~RpcClient()
{
	delete this->rpcInvoker;
}

Object* RpcClient::call(cstring name, const ObjectList& args)
{
	return this->rpcInvoker->call(name, args);
}

Object* RpcClient::call(cstring obj, cstring name, const ObjectList& args)
{
	return this->rpcInvoker->call(obj, name, args);
}

void RpcClient::cast(cstring name, const ObjectList& args)
{
	return this->rpcInvoker->cast(name, args);
}

void RpcClient::cast(cstring obj, cstring name, const ObjectList& args)
{
	return this->rpcInvoker->cast(obj, name, args);
}

bool RpcClient::subscribe(cstring event, cstring method)
{
	return this->rpcInvoker->subscribe(event, method);
}

bool RpcClient::unsubscribe(cstring event, cstring method)
{
	return this->rpcInvoker->unsubscribe(event, method);
}

String RpcClient::login(const ObjectList& args)
{
	return this->rpcInvoker->login(args);
}

String RpcClient::logout()
{
	return this->rpcInvoker->logout();
}

String RpcClient::toString() const
{
	return this->rpcInvoker->toString();
}

bool RpcClient::invoke(const Headers& headers, const InputStream& input)
{
	return this->rpcInvoker->invoke(headers, input);
}

void RpcClient::notifyEvent(cstring event, Object* sender, const ObjectList& args)
{
	return this->rpcInvoker->notifyEvent(event, sender, args);
}

}//end of namespace bluemei