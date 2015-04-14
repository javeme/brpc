#pragma once
#include "stdafx.h"
#include "P2pRpcContext.h"
#include "RpcSocket.h"

namespace bluemei{
	
/*
* 点对点RPC远程调用环境(基于TCP Socket)
* @author 李章梅
* @create 2014/7/13
*/
class P2pRpcClient : public P2pRpcContext, public RpcReceiveListener
{
public:
	P2pRpcClient(cstring url, RpcService* dispatcher, 
		AuthChecker* authChecker, cstring serializerType="text/json");
	virtual ~P2pRpcClient();
public:
	virtual bool onReceive(DataPackage& package);
	virtual bool onException(Exception& e);
protected:
	virtual void checkConnected();
protected:
	virtual void onSend(const DataPackage& output);
	virtual DataPackage onSendSynch(const DataPackage& output);

	virtual bool onSerializeException(SerializeException& e);
	virtual bool onReturnCallException(Exception& e);;
protected:
	RpcSocket* rpcSocket;
};

}//end of namespace bluemei