#pragma once
#include "stdafx.h"
#include "Log.h"
#include "P2pRpcConnAcceptor.h"
#include "P2pRpcConnection.h"
#include "ServerSocket.h"
#include "RpcSocketFactory.h"

namespace bluemei{

P2pRpcConnAcceptor::P2pRpcConnAcceptor(RpcServer* server) 
	: RpcConnAcceptor(server), running(false)
{
	checkNullPtr(server);
	url = Url(server->getUrl());
}

void P2pRpcConnAcceptor::run()
{
	Log* logger = Log::getLogger();
	try{
		waitConnection();
	}catch (Exception& e)
	{
		logger->error("Wait for connection error: " + e.toString());
	}
}

void P2pRpcConnAcceptor::waitConnection()
{
	Log* logger = Log::getLogger();

	checkNullPtr(server);

	String proto = url.getProtocol("jmtp");
	String port = url.getPort(RPC_P2P_PORT);

	ServerSocket serverSocket(CodeUtil::str2Int(port));
	HashMap<String,String> paras;
	paras.put("timeout", String::format("%u",server->getTimeout()));

	this->running = true;
	while(this->running)
	{
		RpcSocket* rpcSocket = RpcSocketFactory::getRpcSocket(proto);
		try{
			logger->debug("wait for connection @" + url.toString());
			BRpcUtil::debug("====wait for connection...\n");
			rpcSocket->acceptWith(&serverSocket, paras);
			BRpcUtil::debug("====connection accepted: %s\n", rpcSocket->toString().c_str());
			logger->debug("connection accepted: " + rpcSocket->toString());
			this->addConnection(rpcSocket);
		}catch (Exception& e)
		{
			delete rpcSocket;
			logger->warn("connection error: " + e.toString());
		}
	}
}

void P2pRpcConnAcceptor::addConnection(RpcSocket* rpcSocket)
{
	checkNullPtr(server);
	checkNullPtr(rpcSocket);
	P2pRpcConnection* conn = new P2pRpcConnection(
		server->getUrl(),
		server->getDispatcher(), 
		server->getAuthChecker(), 
		server->getSerializerType(),
		server->getTimeout(),
		rpcSocket);
	server->addConnection(conn);
}

void P2pRpcConnAcceptor::stop()
{
	this->running = false;
	this->wait();
}

}//end of namespace bluemei


