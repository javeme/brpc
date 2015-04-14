#include "StdAfx.h"
#include "RpcSocket.h"
#include "Date.h"

namespace bluemei{

RpcSocket::RpcSocket()
{
	m_bWaitData=false;
	dataListener=nullptr;

	dataHookHandler=nullptr;
}

RpcSocket::~RpcSocket()
{
	dataListener=nullptr;

	dataHookHandler=nullptr;
}

void RpcSocket::setReceiveListener( RpcReceiveListener* listener )
{
	dataListener=listener;
}

void RpcSocket::setDataHookHandler( RpcDataHookHandle* hook )
{
	dataHookHandler=hook;
}

bool RpcSocket::notifyReceive(DataPackage& input)
{
	if (dataListener!=nullptr)
	{
		if (m_bWaitData && input.getId()==m_idWaitData)
		{
			m_recvPacketList.addToLast(input);
			m_waitLock.signal();
			return true;
		}
		else			
			return dataListener->onReceive(input);
	}
	return false;
}

bool RpcSocket::notifyException(Exception& e)
{
	if (dataListener!=nullptr)
	{
		return dataListener->onException(e);
	}
	return false;
}

DataPackage RpcSocket::sendSynch(const DataPackage& output)
{
	m_bWaitData=true;
	m_idWaitData=output.getId();
	m_recvPacketList.clear();

	this->send(output);
	m_waitLock.wait();

	DataPackage result;
	m_recvPacketList.removeFirstElement(result);

	m_bWaitData=false;
	return result;
}

void RpcSocket::notifyHookReceived(cstring name,const DataPackage& data,long time)
{
	if (dataHookHandler!=nullptr)
	{
		if(time<=0)
			time=(long)Date::getCurrentTime().getTotalMillSecond();
		return dataHookHandler->onReceived(name,data,time);
	}
}

void RpcSocket::notifyHookSent(cstring name,const DataPackage& data,long time)
{
	if (dataHookHandler!=nullptr)
	{
		if(time<=0)
			time=(long)Date::getCurrentTime().getTotalMillSecond();
		return dataHookHandler->onSent(name,data,time);
	}
}


}//end of namespace bluemei