#pragma once
#include "Object.h"
#include "ThreadPool.h"
#include "IOCompletionPort.h"
#include "IOCPEvent.h"
#include "ServerSocket.h"
#include "HashMap.h"


namespace bluemei{

#define EVENT_ALL (EVENT_ERR|EVENT_IN|EVENT_ET)

class BLUEMEILIB_API IOCPModel : public Object
{
public:
	IOCPModel();
	virtual ~IOCPModel();
public:
	//设置事件处理器
	virtual void setEventHandle(IOCPEventHandle *e);
	//发送数据(异步)
	virtual void send(const byte* buf,unsigned int len,socket_t sock);

	//服务端
	virtual void listen(int port);
	virtual void unlisten();//stop

	//客户端
	virtual void connect(cstring ip,int port);
	virtual void disconnect();//stop
protected:
	virtual void start();
	virtual void stop();

	virtual bool notifyException(Exception& e);
protected:
	int getLastError();

	bool addClient(socket_t sock){
		socket_t v=sock;
		return clientSockets.put(sock,v);
	}
	bool removeClient(socket_t sock){
		m_oIOCompletionPort.unregisterEvents(EVENT_ACCEPT|EVENT_ALL,sock);
		socket_t v;
		return clientSockets.remove(sock,v);
	}
protected:
	IOCPEventHandle* m_pIOCPEventHandler;
	IOCompletionPort m_oIOCompletionPort;

	ServerSocket listenSocket;
	HashMap<socket_t,socket_t> clientSockets;
private:
	ThreadPool* m_pIOThreadPool;

	Thread* m_pEventThread;
	volatile bool m_bRunning;
};

}//end of namespace bluemei