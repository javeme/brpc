#pragma once
#include "stdafx.h"
#include "Log.h"
#include "P2pRpcConnAcceptor.h"
#include "P2pRpcConnection.h"
#include "ServerSocket.h"
#include "RpcSocketFactory.h"


namespace brpc{

P2pRpcConnAcceptor::P2pRpcConnAcceptor(RpcServer* server) 
	: RpcConnAcceptor(server), running(false), pool(20)
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

	HashMap<String,String> paras;
	paras.put("timeout", String::format("%u",server->getTimeout()));
	paras.put("noDelay", "true");

	ServerSocket serverSocket(CodeUtil::str2Int(port));
	
	this->running = true;
	while(this->running)
	{
		//static int count=0; count++; if(count>10) break;
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
			logger->warn("connection error: " + e.toString());
			delete rpcSocket;
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
	this->server->addConnection(conn);
	//add to thread pool and poll data
	this->pool.addSocket(rpcSocket);
}

void P2pRpcConnAcceptor::stop()
{
	this->running = false;
	this->wait();
}

void P2pRpcConnAcceptor::wait()
{ 
	RpcConnAcceptor::wait();
	this->pool.stopAndWait();
}

}//end of namespace bluemei


