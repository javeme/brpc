#pragma once
#include "stdafx.h"
#include "RpcSocketManager.h"
#include "RpcOnJmtpSocket.h"
#include "RpcOnHttpSocket.h"

namespace bluemei{


RpcSocket* RpcSocketManager::getRpcSocket(const String& name)
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

}//end of namespace bluemei