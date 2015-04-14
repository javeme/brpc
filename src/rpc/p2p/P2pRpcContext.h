#pragma once
#include "stdafx.h"
#include "Object.h"
#include "ByteBuffer.h"
#include "AbstructRpcContext.h"

namespace bluemei{

const static cstring RPC_P2P_PORT = "8912";

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
class P2pRpcContext : public AbstructRpcContext
{
public:
	P2pRpcContext(cstring url, RpcService* dispatcher, 
		AuthChecker* authChecker, cstring serializerType);
	virtual ~P2pRpcContext();
public:
	//virtual String getPeerUrl() const =0;
public:
	virtual bool subscribe(cstring event, cstring method);
	virtual bool unsubscribe(cstring event, cstring method);
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
	int timeout;//ms
};


}//end of namespace bluemei