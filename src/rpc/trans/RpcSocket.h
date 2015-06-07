#pragma once
#include "Object.h"
#include "RpcException.h"
#include "ResourceLock.h"
#include "LinkedList.cpp"
#include "System.h"
#include "DataPackage.h"
#include "RpcDataHookHandler.h"

namespace bluemei{

typedef unsigned short word;
typedef unsigned int dword;


/*
* RPC接收数据监听器
* @author 李章梅
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
* @author 李章梅
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

	LinkedList<DataPackage> m_recvPacketList;
	bool m_bWaitData;
	String m_idWaitData;
	SyncLock m_waitLock;
	unsigned int timeout;
};

class Url : public Object
{
public:
	Url(const String& url="");
	virtual ~Url() {}
	
public:
	virtual String toString()const { return url; }

	String getProtocol(const String& deft) const { 
		if(protocol.empty())
			return deft;
		return protocol; 
	}
	String getIp(const String& deft="127.0.0.1") const { 
		if(ip.empty())
			return deft;
		return ip; 
	}
	String getPort(const String& deft) const { 
		if(port.empty())
			return deft;
		return port; 
	}
	String getPath(const String& deft="") const { 
		if(path.empty())
			return deft;
		return path; 
	}
protected:
	virtual void parseUrl();
protected:
	String url;

	String protocol;
	String ip;
	String port;
	String path;
};

}//end of namespace bluemei