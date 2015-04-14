#pragma once
#include "RpcSocket.h"
#include "ClientSocket.h"


namespace bluemei{


class RpcOnJmtpSocket : public RpcSocket
{
public:
	RpcOnJmtpSocket(void);
	virtual ~RpcOnJmtpSocket(void);
public:
	virtual void connect(const HashMap<String,String>& paras) throw(RpcException);
	virtual void close() throw(RpcException);

	virtual void send(const DataPackage& package) throw(RpcException);
public:
	virtual void startReceiveThread();
	virtual void stopReceiveThread();

	virtual bool isRecving() const;
	virtual bool isAlive() const;
protected:
	ClientSocket* m_pSocket;
	CriticalLock m_sendLock;

	Thread* m_pRecvThread;
	bool m_bRecving;
};


}//end of namespace bluemei