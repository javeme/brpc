#include "StdAfx.h"
#include "RpcSocket.h"
#include "Date.h"
#include "BRpcUtil.h"

namespace brpc{

RpcSocket::RpcSocket()
{
	this->isWaitingData=false;
	this->dataListener=nullptr;

	this->dataHookHandler=nullptr;

	this->timeout=0;
}

RpcSocket::~RpcSocket()
{
	this->dataListener=nullptr;

	this->dataHookHandler=nullptr;
}

void RpcSocket::setReceiveListener(RpcReceiveListener* listener)
{
	this->dataListener=listener;
}

void RpcSocket::setDataHookHandler(RpcDataHookHandler* hook)
{
	this->dataHookHandler=hook;
}

bool RpcSocket::notifyReceive(DataPackage& input)
{
	if (this->dataListener!=nullptr)
	{
		if (this->isWaitingData && input.getId()==this->idOfWaitData)
		{
			this->recvPacketList.addToLast(input);
			this->waitLock.signal();
			return true;
		}
		else
		{
			return this->dataListener->onReceive(input);
		}
	}
	else
		BRpcUtil::debug("RpcSocket.notifyReceive: dataListener is null\n");
	return false;
}

bool RpcSocket::notifyException(Exception& e)
{
	if (this->dataListener!=nullptr)
	{
		return this->dataListener->onException(e);
	}
	return false;
}

DataPackage RpcSocket::sendSynch(const DataPackage& output)
{
	this->isWaitingData=true;
	this->idOfWaitData=output.getId();
	this->recvPacketList.clear();

	this->send(output);
	unsigned int timeout = this->timeout * 3;
	if(timeout == 0)
		timeout = INFINITE;
	if(!this->waitLock.wait(timeout))
		throwpe(TimeoutException(timeout));

	DataPackage result;
	this->recvPacketList.removeFirstElement(result);

	this->isWaitingData=false;
	return result;
}

void RpcSocket::notifyHookReceived(cstring name,const DataPackage& data,long time)
{
	if (this->dataHookHandler!=nullptr)
	{
		if(time<=0)
			time=(long)Date::getCurrentTime().getTotalMillSecond();
		return this->dataHookHandler->onReceived(name,data,time);
	}
}

void RpcSocket::notifyHookSent(cstring name,const DataPackage& data,long time)
{
	if (this->dataHookHandler!=nullptr)
	{
		if(time<=0)
			time=(long)Date::getCurrentTime().getTotalMillSecond();
		return this->dataHookHandler->onSent(name,data,time);
	}
}

void RpcSocket::notifyHookError(cstring name, cstring error)
{
	if (this->dataHookHandler!=nullptr)
	{
		return this->dataHookHandler->onError(name,error,dataListener);
	}
}

}//end of namespace brpc