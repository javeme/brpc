#pragma once
#include "stdafx.h"
#include "FuncDispatcher.h"
#include "RpcContext.h"


namespace brpc{

class RpcEventHandler;

class RpcService : public Object
{
public:
	RpcService(cstring name = "");
	virtual ~RpcService();
public:
	virtual String toString() const;
public:
	virtual String ping();
	virtual String echo(cstring str);
	virtual String help();

	virtual std::vector<String> listMethods();
	virtual std::vector<String> listVars();
	virtual std::vector<String> listServices();
	virtual std::vector<String> listEextendServices();

	virtual String signatureOf(cstring method);
	virtual String typeOfVar(cstring var);
public:
	virtual bool subscribe(cstring event, cstring method, RpcContext* ctx);
	virtual bool unsubscribe(cstring event, cstring method, RpcContext* ctx);
public:
	//call service method
	virtual Object* call(RpcContext* ctx,
		cstring obj, cstring name, const ObjectList& args);

	//merge other service to this service
	virtual bool extendService(RpcService* service);
	virtual bool excludeService(RpcService* service);

	//append other service to this service as sub-service
	virtual bool addSubSevice(RpcService* service);
	virtual RpcService* removeSubSevice(cstring name);
	virtual RpcService* getSubService(cstring name);
	virtual bool hasSubService(cstring name);

	virtual void publishEvent(cstring name, const ObjectList& args);
protected:
	virtual bool isNeedContext(cstring obj, cstring name)const;
	virtual bool isThisService(cstring name) const;

public:
	//service name
	virtual cstring name() const { return serviceName; }
	//virtual void setName(cstring val) { m_name = val; }

	//service version
	virtual cstring version();

	RpcEventHandler* getEventHandler() const { return eventHandler; }
	void setEventHandler(RpcEventHandler* val) { eventHandler = val; }

public:
	template <typename Func>
	bool regFunction(cstring funcName, Func func) {
		return dispatcher.registerFunction(funcName, func);
	}
	template <typename Func>
	bool regFunctionWithMemberName(cstring funcName, Func func) {
		String fname = funcName;
		fname = fname.splitWith("::").getLast();
		return dispatcher.registerFunction(fname, func);
	}
	template <typename Func>
	void regFunctionWithException(cstring funcName, Func func) {
		if(!regFunctionWithMemberName(funcName, func)) {
			String msg = String("failed to register function: ") + funcName;
			throwpe(RuntimeException(msg));
		}
	}

	#define asnf(name,fun) regFunctionWithException(name, fun)
	#define as(fun) regFunctionWithException(_CODE2STRING(fun), &fun)

	bool regObject(cstring name, Object* obj);
	bool unregObject(cstring name);

	bool setNeedContext(cstring name);
protected:
	String serviceName;
	FuncDispatcher dispatcher;

	HashMap<String, RpcService*> extendServices;
	HashMap<String, RpcService*> subServices;
protected:
	HashMap<String, bool> needCtxFuncMap;
	RpcEventHandler* eventHandler;
};


class RpcEventHandler : public Object
{
public:
	virtual void onEvent(cstring event, Object* sender, const ObjectList& args)=0;
};


template<>
struct TypeObjectHelper<RpcContext*, true>
{
	static bool deletePtr(RpcContext*& ptr)
	{
		ptr = null;
		return false;
	}
	static RpcContext* setNull(RpcContext*& ptr)
	{
		RpcContext* val = ptr;
		ptr = null;
		return val;
	}
};

}//end of namespace bluemei