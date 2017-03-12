#pragma once
#include "blib/ClientSocket.h"
#include "blib/Thread.h"
#include "src/rpc/trans/RpcSocket.h"


namespace brpc{

//tcp socket
class RpcOnTcpSocket : public RpcSocket
{
public:
	RpcOnTcpSocket(void);
	virtual ~RpcOnTcpSocket(void);
public:
	virtual void connect(const HashMap<String,String>& paras) throw(Exception);
	virtual void acceptWith(Object* server,
			const HashMap<String,String>& paras) throw(Exception);
	virtual void close() throw(Exception);
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
	ClientSocket* clientSocket;

	Thread* recvThread;
	volatile bool recving;
	volatile bool hasError;
	bool inServer;  // is this in server side

	volatile int timeoutCount;
	const static int maxTimeoutCount = 10;
};

}//end of namespace brpc
