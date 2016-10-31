#pragma once
#include "src/rpc/trans/RpcSocket.h"
#include "src/rpc/trans/RpcOnTcpSocket.h"
#include "src/rpc/trans/jmtp/JmtpHeader.h"
#include "src/rpc/trans/jmtp/JmtpException.h"


namespace brpc{

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
};


}//end of namespace brpc