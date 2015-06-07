#pragma once
#include "RpcSocket.h"
#include "RpcOnTcpSocket.h"
#include "HttpHeader.h"
#include "HttpException.h"

namespace bluemei{

//http socket 
class RpcOnHttpSocket : public RpcOnTcpSocket
{
public:
	RpcOnHttpSocket(void);
	virtual ~RpcOnHttpSocket(void);
public:
	virtual void send(const DataPackage& package) throw(RpcException);
	virtual void receive() throw(RpcException);
public:
	virtual void sendResponse(HttpResponse::Status status, const String& pkgId);
protected:
	CriticalLock m_sendLock;
};

}//end of namespace bluemei