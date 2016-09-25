#pragma once
#include "stdafx.h"
#include "RpcService.h"
#include "RpcContext.h"

namespace brpc{

const static cstring SYSTEM_OBJ = "system";
const static cstring SYSTEM_CLS = "RpcService";

RpcService::RpcService(cstring name) :
	dispatcher(name), serviceName(name), eventHandler(null)
{
	(void)regObject(SYSTEM_OBJ, this);
	as(RpcService::ping);
	as(RpcService::echo);
	as(RpcService::help);

	as(RpcService::listMethods);
	as(RpcService::listVars);
	as(RpcService::listServices);
	as(RpcService::listEextendServices);

	as(RpcService::signatureOf);
	as(RpcService::typeOfVar);

	as(RpcService::subscribe);
	as(RpcService::unsubscribe);

	if(serviceName.length() > 0)
		(void)regObject(name, this);

	setNeedContext("subscribe");
	setNeedContext("unsubscribe");
}

RpcService::~RpcService()
{
	if(serviceName.length() > 0)
		(void)unregObject(serviceName);
	(void)unregObject(SYSTEM_OBJ);
}

cstring RpcService::version()
{
	return "1.0";
}

String RpcService::toString() const
{
	return "RpcService: " + serviceName;
}

bool RpcService::regObject(cstring name, Object* obj)
{
	return dispatcher.registerVar(name, obj);
}

bool RpcService::unregObject(cstring name)
{
	return dispatcher.unregisterVar(name);
}

bool RpcService::setNeedContext(cstring name)
{
	return needCtxFuncMap.put(name, true);
}

bool RpcService::isNeedContext(cstring obj, cstring name) const
{
	if(needCtxFuncMap.contain(name) && isThisService(obj)){
		return needCtxFuncMap.getDefault(name, false);
	}
	return false;
}

bool RpcService::isThisService(cstring name) const
{
	String fthis(name);
	return fthis == serviceName ||  fthis == SYSTEM_OBJ;
}

Object* RpcService::call(RpcContext* ctx,
	cstring obj, cstring name, const ObjectList& args)
{
	String fthis = obj;
	String fname = name;
	String fsub;
	/*//system, RpcService.ping
	if(String(SYSTEM_OBJ) == fthis)
		fname = String::format("%s.%s", SYSTEM_CLS, name);*/

	if(fthis == "")
	{
		if(fname.contain(".")){
			int pos = fname.find(".");
			fthis = fname.substring(0, pos);
			fname = fname.substring(pos+1);

			pos = fname.rfind(".");
			if(pos > 0){
				fsub = fname.substring(0, pos);
				fname = fname.substring(pos+1);
			}
		}
	}
	else if(fthis.contain("."))
	{
		int pos = fthis.find(".");
		fsub = fthis.substring(pos+1);
		fthis = fthis.substring(0, pos);
	}

	if(fsub.length() > 0){
		//sub-service
		if(hasSubService(fsub) && isThisService(fthis)){
			RpcService* subService = getSubService(fsub);
			checkNullPtr(subService);
			return subService->call(ctx, subService->name(), fname, args);
		}
		//sub-object
		else
			fthis = fthis + "." + fsub;
	}

	if(isNeedContext(fthis, fname)) {
		ObjectList argsWithCtx = args;
		argsWithCtx.add(ctx);
		Object* result = dispatcher.call(fthis, fname, argsWithCtx);
		return result;
	}
	//this service
	return dispatcher.call(fthis, fname, args);
}

String RpcService::ping()
{
	return String::format("BRPC %s/%s", name(), version());
}

String RpcService::echo(cstring str)
{
	return str;
}

String RpcService::help()
{
	String thisList = dispatcher.functionsAsString();

	StringBuilder sb(thisList.length() + subServices.size() * 20);

	sb.append("service:\n");

	auto ss = listServices();
	for(auto i = ss.begin(); i != ss.end(); ++i) {
		sb.append(i->c_str());
		sb.append("\n");
	}

	sb.append("method:\n");
	sb.append(thisList);
	return sb.toString();
}

std::vector<String> RpcService::listMethods()
{
	return dispatcher.listFunctions();
}

std::vector<String> RpcService::listVars()
{
	return dispatcher.listVars();
}

std::vector<String> RpcService::listServices()
{
	std::vector<String> services;
	//services.push_back(name());//self

	auto i = subServices.iterator();
	while(i->hasNext()) {
		//sub-service name
		//services.push_back(String::format("%s.%s", name(), i->next().key.c_str()));
		services.push_back(i->next().key);
	}

	return services;
}

std::vector<String> RpcService::listEextendServices()
{
	std::vector<String> services;

	auto i = extendServices.iterator();
	while(i->hasNext()) {
		//extend-service name
		services.push_back(i->next().key);
	}

	return services;
}

String RpcService::signatureOf(cstring method)
{
	auto funcList = dispatcher.findFunction(method);
	StringBuilder sb(funcList.size() * 20);

	for(auto itor = funcList.begin(); itor != funcList.end(); ++itor) {
		auto& func = *itor;
		if (func) {
			if(itor != funcList.begin())
				sb.append("|");
			sb.append(func->toString());
		}
	}
	if(sb.length() == 0)
		throw NotFoundException(String::format("no method named '%s'", method));
	return sb.toString();
}

String RpcService::typeOfVar(cstring var)
{
	return ObjectRef(var, &dispatcher).objectType();
}

bool RpcService::subscribe(cstring event, cstring method, RpcContext* ctx)
{
	checkNullPtr(ctx);
	return ctx->getInvoker().subscribe(event, method);
}

bool RpcService::unsubscribe(cstring event, cstring method, RpcContext* ctx)
{
	checkNullPtr(ctx);
	return ctx->getInvoker().unsubscribe(event, method);
}

bool RpcService::extendService(RpcService* service)
{
	checkNullPtr(service);
	if(extendServices.put(service->name(), service)) {
		bool success = dispatcher.extend(&service->dispatcher);
		if(!success)
			extendServices.remove(service->name(), service);
		return success;
	}
	return false;
}

bool RpcService::excludeService(RpcService* service)
{
	checkNullPtr(service);
	if(extendServices.remove(service->name(), service)) {
		bool success = dispatcher.exclude(&service->dispatcher);
		if(!success)
			extendServices.put(service->name(), service);
		return success;
	}
	return false;
}

bool RpcService::addSubSevice(RpcService* service)
{
	checkNullPtr(service);
	return subServices.put(service->name(), service);
}

RpcService* RpcService::removeSubSevice(cstring name)
{
	RpcService* service = null;
	(void)subServices.remove(service->name(), service);
	return service;
}

RpcService* RpcService::getSubService(cstring name)
{
	//return subServices.getDefault(name, null);
	String fthis = name;
	String fsub;
	int pos = fthis.find(".");
	if(pos > 0){
		fsub = fthis.substring(pos+1);
		fthis = fthis.substring(0, pos);
	}

	RpcService* subService = subServices.getDefault(fthis, null);
	if(subService)
	{
		if(fsub.length() > 0)
			return subService->getSubService(fsub);
		else
			return subService;
	}
	return null;
}

bool RpcService::hasSubService(cstring name)
{
	//return subServices.contain(name);
	return (getSubService(name) != null);
}

void RpcService::publishEvent(cstring name, const ObjectList& args)
{
	if(eventHandler){
		String event = serviceName + "." + name;
		eventHandler->onEvent(event, this, args);
	}
}

}//end of namespace brpc


