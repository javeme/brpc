#pragma once
#include "stdafx.h"
#include "P2pRpcContext.h"

namespace bluemei{


P2pRpcContext::P2pRpcContext(cstring url, RpcService* dispatcher, 
	AuthChecker* authChecker, cstring serializerType)
	: url(url), AbstructRpcContext(dispatcher, authChecker, serializerType)
{
	this->timeout = 1000 * 10;//10s
	this->url = this->url.trim();
	parseUrl(this->url);
}

P2pRpcContext::~P2pRpcContext()
{
	;
}

void P2pRpcContext::parseUrl(const String& url)
{
	protocol = "jmtp";
	ip = "localhost";
	port = RPC_P2P_PORT;

	String addr;
	if(url.contain("://")) //jmtp://127.0.0.1:8080
	{
		auto list = url.splitWith("://");
		protocol = list[0];
		addr = list[1];
	}
	else
		addr = url;
	
	if(addr.contain(":")){ //127.0.0.1:8080
		auto list = url.splitWith(":");
		ip = list[0];
		port = list[1];
	}
	else{ //127.0.0.1
		ip = addr;
	}
}

void P2pRpcContext::onNotifyEvent(cstring event, const ObjectList& args)
{
	if(eventMap.contain(event)){
		String method = eventMap.getDefault(event, "");
		//this->cast(method, args);
		Headers headers;
		headers.put(KEY_RESPONSE, "true");
		;
		notifyEvent(headers, method, args);
	}
}

bool P2pRpcContext::subscribe(cstring event, cstring method)
{
	return eventMap.put(event, method);
}

bool P2pRpcContext::unsubscribe(cstring event, cstring method)
{
	String val;
	if(eventMap.remove(event, val))
		return val == method;
	return false;
}

}//end of namespace bluemei