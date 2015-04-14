#pragma once
#include "stdafx.h"
#include "ErrorHandler.h"
#include "P2pRpcClient.h"
#include "RpcSocketManager.h"

namespace bluemei{


P2pRpcClient::P2pRpcClient(cstring url, RpcService* dispatcher, 
	AuthChecker* authChecker, cstring serializerType)
	: P2pRpcContext(url, dispatcher, authChecker, serializerType), rpcSocket(null)
{
	;
}

P2pRpcClient::~P2pRpcClient()
{
	if(rpcSocket){
		delete rpcSocket;
	}
}

void P2pRpcClient::checkConnected()
{
	if (rpcSocket == null){
		rpcSocket = RpcSocketManager::getRpcSocket(protocol);
		if (rpcSocket == null)
			throwpe(RpcException("invalid url: protocol " + protocol));
		rpcSocket->setReceiveListener(this);
	}

	if (!rpcSocket->isAlive()){
		HashMap<String,String> paras;
		paras.put("ip", ip);
		paras.put("port", port);
		paras.put("timeout", String() + timeout);
		rpcSocket->connect(paras);
	}
}

bool P2pRpcClient::onReceive(DataPackage& package)
{
	return inputCall(package.headers, package.body);
}

bool P2pRpcClient::onException(Exception& e)
{
	ErrorHandler::handle(e);
	return false;
}

void P2pRpcClient::onSend(const DataPackage& output)
{
	rpcSocket->send(output);
}

DataPackage P2pRpcClient::onSendSynch(const DataPackage& output)
{
	return rpcSocket->sendSynch(output);
}

bool P2pRpcClient::onSerializeException(SerializeException& e)
{
	ErrorHandler::handle(RpcException("invalid request " + e.toString()));
	return false;	
}

bool P2pRpcClient::onReturnCallException(Exception& e)
{
	ErrorHandler::handle(e);
	return false;
}


}//end of namespace bluemei