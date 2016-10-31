#pragma once
#include "src/rpc/trans/RpcSocket.h"
#include "src/util/BRpcUtil.h"


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
	ScopedLock lock(this->sendLock);

	if (this->dataListener!=nullptr)
	{
		if (this->isWaitingData && input.getId()==this->idOfWaitData)
		{
			this->recvPacket = input;
			// to ensure that:
			// 1.if there is no waiter, we don't signal
			// 2.if there is a waiter, we send a signal
			// so, we use notify() instead of signal()
			// signal() may lead to send mult signals but
			// no consumer(such as all consumers timeout)
			this->waitLock.notify();
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
	ScopedLock lock(this->sendLock);

	this->isWaitingData=true;
	this->idOfWaitData=output.getId();

	this->send(output);
	unsigned int timeout = this->timeout + 100;
	if(this->timeout == 0)
		timeout = INFINITE;
	if(!this->waitLock.wait(this->sendLock, timeout))
		throwpe(TimeoutException(timeout));

	DataPackage& result = this->recvPacket;
	if(result.getId() != output.getId()) {
		// if another thread send a request, idOfWaitData may changed
		// TODO: should we support multi-thread to send? if so, we should
		// add a map instead of idOfWaitData, like map<id, object(lock, value)>
		// when a response reached:
		//   map(id).value=response;
		//   map(id).lock.notify();
		String msg = "Unexpected response received, there may be another "\
			"thread sending a request? if so please use async-send instead!";
		throwpe(RpcException(msg));
	}

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