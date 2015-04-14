#pragma once
#include "stdafx.h"
#include "Object.h"


namespace bluemei{

class RpcService;

class RpcServer : public Object
{
public:
	RpcServer();
	virtual ~RpcServer();
public:
	virtual void run(RpcService* service)=0;
	virtual void stop()=0;
};

}//end of namespace bluemei