#pragma once
#include "src/rpc/client/RpcClient.h"
#include "src/rpc/server/RpcService.h"


namespace brpc{

class RpcApi : public Object
{
public:
	RpcApi(cstring name, RpcApi* parent=null);
	RpcApi(cstring name, RpcClient* rpcClient, const ObjectList& loginArgs);
	virtual ~RpcApi();
public:
	virtual void init(RpcClient* rpcClient, const ObjectList& loginArgs);
	virtual void init(RpcApi* parent);
	virtual void logout();
	RpcInvoker* invoker() const { return rpcClient; }
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
		checkNullPtr(rpcClient);
		ScopePointer<Object> result = rpcClient->call(object, method, args);
		return valueOf<T>(result);
	}

	void cast(cstring method, const ObjectList& args) {
		return cast(object(), method, args);
	}
	void cast(cstring object, cstring method, const ObjectList& args) {
		checkNullPtr(rpcClient);
		return rpcClient->cast(object, method, args);
	}
public:
	virtual cstring name() const { return serviceName; }
	virtual String object() const;
protected:
	String serviceName;
	RpcClient* rpcClient;
	//String objId;
	RpcApi* parent;
	HashMap<String, RpcApi*> subServices;
protected:
	const ObjectList nullArgs;
};

}//end of namespace brpc