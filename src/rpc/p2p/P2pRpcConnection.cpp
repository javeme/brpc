#pragma once
#include "blib/ErrorHandler.h"
#include "src/rpc/p2p/P2pRpcConnection.h"
#include "src/rpc/trans/RpcSocketFactory.h"


namespace brpc{

P2pRpcConnection::P2pRpcConnection(cstring url, RpcService* dispatcher,
	AuthChecker* authChecker, cstring serializerType,
	unsigned int timeout, RpcSocket* sock)
	: url(url), rpcSocket(sock), timeout(timeout),
	  RpcConnection(dispatcher, authChecker, serializerType)
{
	this->url = this->url.trim();
	parseUrl(this->url);

	//rpcSocket is null if this is a client, else not null(server).
	if (this->rpcSocket == null){
		this->rpcSocket = RpcSocketFactory::getRpcSocket(protocol);
		if (this->rpcSocket == null)
			throwpe(RpcException("invalid url: protocol " + protocol));
	}
	this->rpcSocket->setReceiveListener(this);
}

P2pRpcConnection::~P2pRpcConnection()
{
	if(this->rpcSocket){
		delete this->rpcSocket;
	}
}

void P2pRpcConnection::parseUrl(const String& url)
{
	protocol = "jmtp";
	ip = "localhost";
	port = RPC_P2P_PORT;

	Url parser(url);
	protocol = parser.getProtocol(protocol);
	ip = parser.getHost(ip);
	port = parser.getPort(port);
}

void P2pRpcConnection::onNotifyEvent(cstring event, const ObjectList& args)
{
	if(eventMap.contain(event)){
		//remote method name
		String method = eventMap.getDefault(event, "");
		//this->cast(method, args);
		Headers headers;
		headers.put(KEY_RESPONSE, "true");
		headers.put(KEY_CONTENT_TYPE, this->serializerType);
		;
		sendEvent(headers, method, args);
	}
}

bool P2pRpcConnection::subscribe(cstring event, cstring method)
{
	return eventMap.put(event, method);
}

bool P2pRpcConnection::unsubscribe(cstring event, cstring method)
{
	String val;
	if(eventMap.remove(event, val))
		return val == method;
	return false;
}

bool P2pRpcConnection::checkConnected()
{
	if (!this->rpcSocket->isAlive()){
		if (this->rpcSocket->isInServer())
			return false;

		HashMap<String,String> paras;
		paras.put("ip", ip);
		paras.put("port", port);
		paras.put("timeout", String::format("%u",timeout));
		this->rpcSocket->connect(paras);
	}
	return true;
}

void P2pRpcConnection::setDataHookHandler(RpcDataHookHandler* handler)
{
	this->rpcSocket->setDataHookHandler(handler);
}

bool P2pRpcConnection::onReceive(DataPackage& package)
{
	return invoke(package.headers, package.body);
}

bool P2pRpcConnection::onException(Exception& e)
{
	ErrorHandler::handle(e);
	return false;
}

void P2pRpcConnection::onSend(const DataPackage& output)
{
	this->rpcSocket->send(output);
}

DataPackage P2pRpcConnection::onSendSynch(const DataPackage& output)
{
	return this->rpcSocket->sendSynch(output);
}

bool P2pRpcConnection::onSerializeException(SerializeException& e)
{
	ErrorHandler::handle(e);
	return false;
}

bool P2pRpcConnection::onReturnCallException(Exception& e)
{
	ErrorHandler::handle(e);
	return false;
}

String P2pRpcConnection::toString() const
{
	return this->rpcSocket->toString();
}

}//end of namespace brpc