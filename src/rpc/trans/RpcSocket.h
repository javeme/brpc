#pragma once
#include "blib/ConditionLock.h"
#include "blib/Date.h"
#include "blib/Log.h"
#include "src/rpc/trans/DataPackage.h"
#include "src/util/RpcException.h"
#include "src/rpc/trans/RpcDataHookHandler.h"
#include "src/util/Url.h"


namespace brpc{

typedef unsigned short word;
typedef unsigned int dword;

/*
* RPC接收数据监听器
* @author Javeme
* @create 2012/5/28
*/
class RpcReceiveListener : public virtual Object
{
public:
	virtual bool onReceive(DataPackage& input)=0;
	virtual bool onException(Exception& e)=0;
};

/*
* RPC传输层处理器
* RPC传输层抽象接口(RpcConnection)
*
* @author Javeme
* @create 2012/5/28
*/
class RpcSocket : public Object
{
public:
	RpcSocket();
	virtual ~RpcSocket();
public:
	virtual void connect(const HashMap<String,String>& paras) throw(Exception)=0;
	virtual void acceptWith(Object* server,
			const HashMap<String,String>& paras) throw(Exception)=0;
	virtual void close() throw(Exception)=0;

	virtual void send(const DataPackage& output) throw(Exception)=0;
	virtual void receive() throw(Exception)=0;
	virtual void setReceiveListener(RpcReceiveListener* listener);

	virtual void setDataHookHandler(RpcDataHookHandler* hook);

	virtual bool isAlive() const=0;
	virtual bool isInServer() const=0;

	virtual DataPackage sendSynch(const DataPackage& output) throw(Exception);
protected:
	virtual bool notifyReceive(DataPackage& input);
	virtual bool notifyException(Exception& e);

	virtual void notifyHookReceived(cstring name,const DataPackage& data,long time=0);
	virtual void notifyHookSent(cstring name,const DataPackage& data,long time=0);
	virtual void notifyHookError(cstring name, cstring error);
protected:
	RpcReceiveListener* dataListener;
	RpcDataHookHandler* dataHookHandler;

	DataPackage recvPacket;
	bool isWaitingData;
	String idOfWaitData;
	ConditionLock sendLock;

	unsigned int timeout;
};

}//end of namespace brpc
