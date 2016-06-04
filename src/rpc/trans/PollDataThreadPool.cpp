#pragma once
#include "stdafx.h"
#include "PollDataThreadPool.h"
#include "RpcOnTcpSocket.h"


namespace brpc{

class PollDataTask : public Task
{
public:
	PollDataTask(RpcSocket* socket)
	{
		checkNullPtr(socket);
		this->socket = dynamic_cast<RpcOnTcpSocket*>(socket);
		if(this->socket == null)
			throwpe(Exception("Not supported Socket except RpcOnTcpSocket"));
	}
public:
	virtual void stop() 
	{
		this->socket->stopReceiveLoop();
	}

	virtual void setId( int id ) 
	{
		;
	}

	virtual void taskStarted() 
	{
		;
	}

	virtual int getId() 
	{
		return 0;
	}

	virtual void taskFinished() 
	{
		delete this;
	}

	virtual bool isRunning() 
	{
		return this->socket->isRecving();
	}

	virtual void run()
	{
		this->socket->startReceiveLoop();
	}
protected:
	RpcOnTcpSocket* socket;
};


PollDataThreadPool::PollDataThreadPool(int poolSize) : pool(poolSize)
{
}

PollDataThreadPool::~PollDataThreadPool()
{
}

void PollDataThreadPool::addSocket(RpcSocket* socket)
{
	(void)this->pool.addTask(new PollDataTask(socket));
}

void PollDataThreadPool::stopAndWait()
{
	this->pool.stop();
}

}//end of namespace brpc