#pragma once
#include "stdafx.h"
#include "RpcInvokerFacatory.h"
#include "amqp/AmqpRpcConnection.h"
#include "p2p/P2pRpcConnection.h"
#include "p2p/P2pRpcConnAcceptor.h"

namespace bluemei{
	

RpcInvoker* RpcInvokerFacatory::loadRpcInvoker(cstring url, RpcService* dispatcher, 
	AuthChecker* authChecker, cstring serializerType, unsigned int timeout)
{
	String proto(url);
	if (proto.startWith("amqp://"))
	{
		return new AmqpRpcConnection(url, dispatcher, authChecker, 
			serializerType, timeout);
	}
	else
	{
		return new P2pRpcConnection(url, dispatcher, authChecker, 
			serializerType, timeout);
	}
}

RpcConnAcceptor* RpcInvokerFacatory::loadRpcAcceptor(cstring url, RpcServer* server)
{
	String proto(url);
	if (proto.startWith("amqp://"))
	{
		throw Exception("not implent");
		//return new AmqpRpcConnAcceptor(server);
	}
	else
	{
		return new P2pRpcConnAcceptor(server);
	}
}

}//end of namespace bluemei