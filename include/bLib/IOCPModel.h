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
	virtual void unlisten();

	//客户端
	virtual socket_t connect(cstring ip,int port);
	virtual void disconnect();

	int timeout() const { return m_nTimeout; }
	void setTimeout(int val) { m_nTimeout = val; }
protected:
	virtual void start();
	virtual void stop();

	virtual bool notifyException(Exception& e);
protected:
	int getLastError();

	bool addClient(socket_t sock);
	bool removeClient(socket_t sock);
	void closeAllClients();
protected:
	IOCPEventHandle* m_pIOCPEventHandler;
	IOCompletionPort m_oIOCompletionPort;

	ServerSocket m_listenSocket;
	HashMap<socket_t,socket_t> m_clientSockets;

	int m_nTimeout;
private:
	ThreadPool* m_pIOThreadPool;

	Thread* m_pEventThread;
	volatile bool m_bRunning;
};

}//end of namespace bluemei