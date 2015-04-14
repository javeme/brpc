#pragma once
#include "stdafx.h"
#include "RpcContext.h"
#include "RpcSerializeable.h"
#include "FuncDispatcher.h"
#include "AuthChecker.h"
#include "RpcService.h"
#include "DataPackage.h"


namespace bluemei{


/*
* RPC远程调用环境
* @author 李章梅
* @create 2014/7/13
*/
class AbstructRpcContext : public RpcContext
{
public:
	AbstructRpcContext(RpcService* dispatcher,
		AuthChecker* authChecker, cstring serializerType);
	virtual ~AbstructRpcContext();
public:
	virtual Object* call(cstring name, const ObjectList& args);
	virtual void cast(cstring name, const ObjectList& args);
	
	virtual Object* call(cstring obj, cstring name, const ObjectList& args);
	virtual void cast(cstring obj, cstring name, const ObjectList& args);
	
	virtual void notifyEvent(cstring event, Object* sender, const ObjectList& args);

	virtual String login(const ObjectList& args);
	virtual String logout();
protected:
	virtual Object* onCall(cstring obj, cstring name, const ObjectList& args);
	virtual Object* onCall(cstring name, const ObjectList& args);

	virtual void castMethod(const Headers& headers, const RpcMethod& method);

	virtual void notifyEvent(const Headers& headers, cstring method, 
							 const ObjectList& args);
	virtual void onNotifyEvent(cstring event, const ObjectList& args);
public:
	virtual bool subscribe(cstring event, cstring method);
	virtual bool unsubscribe(cstring event, cstring method);
protected:
	virtual bool doCallResponse(const Headers& headers, const RpcMethod& method, 
								Object* result, int status);
	virtual bool doCall(const Headers& headers, RpcMethod& method);
	virtual bool doEvent(const Headers& headers, RpcMethod& method);
	virtual bool inputCall(const Headers& headers, const InputStream& input);

	virtual void sendCall4Result(const RpcMethod& method, RpcMethod& methodResult);

	virtual void checkAuth(const RpcMethod& method);
protected:
	virtual void checkConnected()=0;

	virtual void onSend(const DataPackage& output)=0;
	virtual DataPackage onSendSynch(const DataPackage& output)=0;

	virtual bool onSerializeException(SerializeException& e)=0;
	virtual bool onReturnCallException(Exception& e)=0;
protected:
	RpcSerializeable* getSerializer(cstring name);
protected:
	String serializerType;
	RpcSerializeable* serializer;

	RpcService* dispatcher;
protected:
	AuthChecker* authChecker;
private:
	String authToken;//client's token from server
};

}//end of namespace bluemei