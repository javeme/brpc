#pragma once
#include "RpcSocket.h"
#include "RpcOnTcpSocket.h"
#include "JmtpHeader.h"
#include "JmtpException.h"

namespace bluemei{

class RpcOnJmtpSocket : public RpcOnTcpSocket
{
public:
	RpcOnJmtpSocket(void);
	virtual ~RpcOnJmtpSocket(void);
public:
	virtual void send(const DataPackage& package) throw(RpcException);
	virtual void receive() throw(RpcException);
public:
	virtual void sendResponse(JmtpHeader::Status status, const String& pkgId);
protected:
	CriticalLock m_sendLock;
};


}//end of namespace bluemei