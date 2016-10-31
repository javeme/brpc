#pragma once
#include "src/rpc/trans/RpcSocketFactory.h"
#include "src/rpc/trans/jmtp/RpcOnJmtpSocket.h"
#include "src/rpc/trans/http/RpcOnHttpSocket.h"


namespace brpc{

RpcSocket* RpcSocketFactory::getRpcSocket(const String& name)
{
	if (name.compare("Jmtp",false))
	{
		return new RpcOnJmtpSocket();
	}
	else if (name.compare("Http",false))
	{
		return new RpcOnHttpSocket();
	}
	else
		return nullptr;
}

}//end of namespace brpc