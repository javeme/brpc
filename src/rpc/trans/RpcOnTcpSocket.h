#pragma once
#include "RpcSocket.h"
#include "ClientSocket.h"


namespace brpc{


//tcp socket 
class RpcOnTcpSocket : public RpcSocket
{
public:
	RpcOnTcpSocket(void);
	virtual ~RpcOnTcpSocket(void);
public:
	virtual void connect(const HashMap<String,String>& paras) throw(RpcException);
	virtual void acceptWith(Object* server, const HashMap<String,String>& paras) throw(RpcException);
	virtual void close() throw(RpcException);
public:
	virtual void startReceiveLoop();
	virtual void stopReceiveLoop();

	virtual void startReceiveThread();
	virtual void stopReceiveThread();

	virtual bool isRecving() const;
	virtual bool isAlive() const;
	virtual bool isInServer() const;

	virtual String toString() const;
protected:
	virtual void initSocket(const HashMap<String,String>& paras);
protected:
	ClientSocket* m_pSocket;

	Thread* m_pRecvThread;
	bool m_bRecving;
	bool m_bError;
	bool m_bInServer;

	int m_nTimeoutCount;
	const static int m_nMaxTimeoutCount = 10;
};

}//end of namespace bluemei