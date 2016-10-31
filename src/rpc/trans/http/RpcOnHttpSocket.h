#pragma once
#include "src/rpc/trans/RpcSocket.h"
#include "src/rpc/trans/RpcOnTcpSocket.h"
#include "src/rpc/trans/http/HttpHeader.h"
#include "src/rpc/trans/http/HttpException.h"


namespace brpc{

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
};

}//end of namespace brpc