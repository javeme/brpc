#include "StdAfx.h"
#include "RpcSocket.h"
#include "Date.h"

namespace bluemei{

RpcSocket::RpcSocket()
{
	m_bWaitData=false;
	dataListener=nullptr;

	dataHookHandler=nullptr;

	timeout = 0;
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

void RpcSocket::setDataHookHandler( RpcDataHookHandler* hook )
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
	unsigned int timeout = this->timeout * 3;
	if(timeout == 0)
		timeout = INFINITE;
	if(!m_waitLock.wait(timeout))
		throwpe(TimeoutException(timeout));

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

void RpcSocket::notifyHookError(cstring name, cstring error)
{
	if (dataHookHandler!=nullptr)
	{
		return dataHookHandler->onError(name,error,dataListener);
	}
}


///////////////////////////////////////////////////////////////////////////
//class Url
Url::Url( const String& url/*=""*/ ) : url(url)
{
	if(!url.empty())
		parseUrl();
}

void Url::parseUrl()
{
	String addr;
	if(url.contain("://")) // http://127.0.0.1:8080
	{
		auto list = url.splitWith("://");
		protocol = list[0];
		addr = list[1];
	}
	else
		addr = url;

	if(addr.contain("/")){ // 127.0.0.1:8080/index.html
		auto list = url.splitWith("/");
		addr = list[0];
		path = list[1];
	}

	if(addr.contain(":")){ // 127.0.0.1:8080
		auto list = url.splitWith(":");
		ip = list[0];
		port = list[1];
	}
	else{ //127.0.0.1
		ip = addr;
	}
}

}//end of namespace bluemei