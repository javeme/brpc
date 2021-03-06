#pragma once
#include "src/rpc/client/RpcApi.h"


namespace brpc{

RpcApi::RpcApi(cstring name, RpcApi* parent)
	: serviceName(name), rpcClient(null), parent(parent)
{
	(void)init(parent);
}

RpcApi::RpcApi(cstring name, RpcClient* rpcClient, const ObjectList& loginArgs)
	: serviceName(name), rpcClient(rpcClient), parent(null)
{
	(void)init(rpcClient, loginArgs);
}

RpcApi::~RpcApi()
{
	this->rpcClient = null;
}

void RpcApi::init(RpcClient* rpcClient, const ObjectList& loginArgs)
{
	checkNullPtr(rpcClient);
	this->rpcClient = rpcClient;
	(void)this->rpcClient->login(loginArgs);
}

void RpcApi::init(RpcApi* parent)
{
	if(parent){
		rpcClient = parent->rpcClient;
		if(!parent->addSubSevice(this))
			throw Exception("add service failed");
	}
}

void RpcApi::logout()
{
	checkNullPtr(rpcClient);
	(void)rpcClient->logout();
}

String RpcApi::ping()
{
	return call<String>("ping", nullArgs);
}

String RpcApi::echo(cstring str)
{
	ObjectList args;
	args.addValue(str);
	return call<String>("echo", args);
}

String RpcApi::help()
{
	return call<String>("help", nullArgs);
}

std::vector<String> RpcApi::listMethods()
{
	return call<std::vector<String>>("listMethods", nullArgs);
}

std::vector<String> RpcApi::listVars()
{
	return call<std::vector<String>>("listVars", nullArgs);
}

std::vector<String> RpcApi::listServices()
{
	return call<std::vector<String>>("listServices", nullArgs);
}

std::vector<String> RpcApi::listEextendServices()
{
	return call<std::vector<String>>("listEextendServices", nullArgs);
}

String RpcApi::signatureOf(cstring method)
{
	ObjectList args;
	args.addValue(method);
	return call<String>("signatureOf", args);
}

String RpcApi::typeOfVar(cstring var)
{
	ObjectList args;
	args.addValue(var);
	return call<String>("typeOfVar", args);
}

bool RpcApi::subscribe(cstring event, cstring method)
{
	ObjectList args;
	args.addValue(event);
	args.addValue(method);
	return call<bool>("subscribe", args);
}

bool RpcApi::unsubscribe(cstring event, cstring method)
{
	ObjectList args;
	args.addValue(event);
	args.addValue(method);
	return call<bool>("unsubscribe", args);
}

bool RpcApi::addSubSevice(RpcApi* service)
{
	checkNullPtr(service);
	return subServices.put(service->name(), service);
}

RpcApi* RpcApi::removeSubSevice(cstring name)
{
	RpcApi* service = null;
	(void)subServices.remove(service->name(), service);
	return service;
}

RpcApi* RpcApi::getSubService(cstring name)
{
	return subServices.getDefault(name, null);
}

String RpcApi::object() const
{
	if(parent)
		return String::format("%s.%s", parent->object().c_str(), name());
	else
		return name();
}

}//end of namespace brpc