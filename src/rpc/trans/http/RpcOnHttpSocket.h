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
	virtual void send(const DataPackage& package) throw(Exception);
	virtual void receive() throw(Exception);
public:
	virtual void sendResponse(HttpResponse::Status status,
			const String& pkgId) throw(Exception);
};

}//end of namespace brpc
