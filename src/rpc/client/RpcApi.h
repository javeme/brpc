#pragma once
#include "stdafx.h"
#include "RpcContext.h"
#include "RpcService.h"

namespace bluemei{


class RpcApi : public Object
{
public:
	RpcApi(cstring name, RpcApi* parent=null);
	RpcApi(cstring name, RpcContext* rpcContext, const ObjectList& loginArgs);
	virtual ~RpcApi();
public:
	virtual void init(RpcContext* rpcContext, const ObjectList& loginArgs);
	virtual void init(RpcApi* parent);
	virtual void logout();
	RpcContext* context() const { return rpcContext; }
public:
	virtual String ping();
	virtual String echo(cstring str);
	virtual String help();

	virtual std::vector<String> listMethods();
	virtual std::vector<String> listVars();
	virtual std::vector<String> listServices();

	virtual String signatureOf(cstring method);	
	virtual String typeOfVar(cstring var);
public:
	virtual bool subscribe(cstring event, cstring method);
	virtual bool unsubscribe(cstring event, cstring method);
public:
	virtual bool addSubSevice(RpcApi* service);
	virtual RpcApi* removeSubSevice(cstring name);
	virtual RpcApi* getSubService(cstring name);
public:
	template<class T>
	T call(cstring method, const ObjectList& args) {
		return call<T>(object(), method, args);
	}
	template<class T>
	T call(cstring object, cstring method, const ObjectList& args) {
		checkNullPtr(rpcContext);
		SmartPtr<Object> result = rpcContext->call(object, method, args);
		return valueOf<T>(result);
	}

	template<class T>
	T cast(cstring method, const ObjectList& args) {
		return cast<T>(object(), method, args);
	}
	template<class T>
	T cast(cstring object, cstring method, const ObjectList& args) {
		checkNullPtr(rpcContext);
		SmartPtr<Object> result = rpcContext->cast(object, method, args);
		return valueOf<T>(result);
	}
public:
	virtual cstring name() const { return serviceName; }
	virtual String object() const;
protected:
	String serviceName;
	RpcContext* rpcContext;
	//String objId;
	RpcApi* parent;
	HashMap<String, RpcApi*> subServices;
protected:
	const ObjectList nullArgs;
};

}//end of namespace bluemei