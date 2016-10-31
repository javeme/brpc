#pragma once
#include "src/rpc/RpcInvoker.h"
#include "src/serializer/RpcSerializeable.h"
#include "src/fun/FuncDispatcher.h"
#include "src/auth/AuthChecker.h"
#include "src/rpc/server/RpcService.h"
#include "src/rpc/trans/DataPackage.h"
#include "src/rpc/trans/RpcDataHookHandler.h"


namespace brpc{

/*
* RPC远程调用环境
* @author Javeme
* @create 2014/7/13
*/
class RpcConnection : public RpcInvoker
{
public:
	RpcConnection(RpcService* dispatcher,
		AuthChecker* authChecker, cstring serializerType);
	virtual ~RpcConnection();
public:
	virtual Object* call(cstring name, const ObjectList& args);
	virtual void cast(cstring name, const ObjectList& args);

	virtual Object* call(cstring obj, cstring name, const ObjectList& args);
	virtual void cast(cstring obj, cstring name, const ObjectList& args);

	virtual void notifyEvent(cstring event, Object* sender, const ObjectList& args);

	virtual String login(const ObjectList& args);
	virtual String logout();
public:
	virtual bool subscribe(cstring event, cstring method);
	virtual bool unsubscribe(cstring event, cstring method);
public:
	virtual void sendEvent(const Headers& headers, cstring method, const ObjectList& args);
	virtual bool invoke(const Headers& headers, const InputStream& input);
public:
	virtual bool checkConnected()=0;

	virtual void setDataHookHandler(RpcDataHookHandler* handler);
protected:
	virtual Object* onCall(cstring obj, cstring name, const ObjectList& args);
	virtual Object* onCall(cstring name, const ObjectList& args);

	virtual void castMethod(const Headers& headers, const RpcMethod& method);

	virtual void onNotifyEvent(cstring event, const ObjectList& args);
protected:
	virtual bool doCallResponse(const Headers& headers, const RpcMethod& method,
								Object* result, int status);
	virtual bool doCall(const Headers& headers, RpcMethod& method);
	virtual bool doEvent(const Headers& headers, RpcMethod& method);

	virtual void sendCall4Result(const RpcMethod& method, RpcMethod& methodResult);

	virtual void checkAuth(const RpcMethod& method);
protected:
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
	String authToken;//client's token from server
};

}//end of namespace brpc