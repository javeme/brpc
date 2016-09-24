#pragma once
#include "stdafx.h"
#include "AuthChecker.h"
#include "RpcService.h"

namespace brpc{

class RpcServer;
class RpcConnAcceptor;

/*
* RPC远程调用器工厂
* @author Javeme
* @create 2014/7/13
*/
class RpcInvokerFacatory : public Object
{
public:
	static RpcInvoker* loadRpcInvoker(String url, RpcService* dispatcher,
		AuthChecker* authChecker, cstring serializerType, unsigned int timeout=0);
	static RpcConnAcceptor* loadRpcAcceptor(String url, RpcServer* server);
};

}//end of namespace brpc