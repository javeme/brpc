#pragma once
#include "stdafx.h"
#include "RpcConnection.h"
#include "RpcSocket.h"


namespace brpc{

const static cstring RPC_P2P_PORT = "8080";//8912

/*class RpcEventListener : public Object
{
private:
	String event;
	String receiver;
	String obj;
	String method;
};*/

/*
* 点对点RPC远程调用环境(基于TCP Socket)
* @author 李章梅
* @create 2014/7/13
*/
class P2pRpcConnection : public RpcConnection, 
	public RpcReceiveListener
{
public:
	P2pRpcConnection(cstring url, RpcService* dispatcher, 
		AuthChecker* authChecker, cstring serializerType,
		unsigned int timeout=0,RpcSocket* rpcSocket=null);
	virtual ~P2pRpcConnection();
public:
	//virtual String getPeerUrl() const =0;
	virtual String toString() const;
public:
	virtual bool subscribe(cstring event, cstring method);
	virtual bool unsubscribe(cstring event, cstring method);
public:
	virtual bool onReceive(DataPackage& package);
	virtual bool onException(Exception& e);
protected:
	virtual bool checkConnected();
	virtual void setDataHookHandler(RpcDataHookHandler* handler);
protected:
	virtual void onSend(const DataPackage& output);
	virtual DataPackage onSendSynch(const DataPackage& output);

	virtual bool onSerializeException(SerializeException& e);
	virtual bool onReturnCallException(Exception& e);
protected:
	virtual void onNotifyEvent(cstring event, const ObjectList& args);
protected:
	virtual void parseUrl(const String& url);
protected:
	String url;
	HashMap<String, String> eventMap;

	String protocol;
	String ip;
	String port;
	unsigned int timeout;
protected:
	RpcSocket* rpcSocket;
};


}//end of namespace bluemei