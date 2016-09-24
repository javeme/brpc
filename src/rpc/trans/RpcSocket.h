#pragma once
#include "DataPackage.h"
#include "RpcException.h"
#include "RpcDataHookHandler.h"
#include "Url.h"


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
	virtual void connect(const HashMap<String,String>& paras) throw(RpcException)=0;//或Map,也可用stl map,但操作稍微麻烦
	virtual void acceptWith(Object* server,const HashMap<String,String>& paras) throw(RpcException)=0;
	virtual void close() throw(RpcException)=0;

	virtual void send(const DataPackage& output) throw(RpcException)=0;
	virtual void receive() throw(RpcException)=0;
	virtual void setReceiveListener(RpcReceiveListener* listener);

	virtual void setDataHookHandler(RpcDataHookHandler* hook);
	
	virtual bool isAlive() const=0;
	virtual bool isInServer() const=0;

	virtual DataPackage sendSynch(const DataPackage& output);
protected:
	virtual bool notifyReceive(DataPackage& input);
	virtual bool notifyException(Exception& e);

	virtual void notifyHookReceived(cstring name,const DataPackage& data,long time=0);
	virtual void notifyHookSent(cstring name,const DataPackage& data,long time=0);
	virtual void notifyHookError(cstring name, cstring error);
protected:
	RpcReceiveListener* dataListener;
	RpcDataHookHandler* dataHookHandler;

	LinkedList<DataPackage> recvPacketList;
	bool isWaitingData;
	String idOfWaitData;
	SyncLock waitLock;
	unsigned int timeout;
};

}//end of namespace brpc