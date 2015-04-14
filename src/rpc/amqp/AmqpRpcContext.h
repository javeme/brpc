#pragma once
#include "stdafx.h"
#include "Object.h"
#include "AbstructRpcContext.h"

namespace bluemei{


/*
* 基于AMQP协议RPC远程调用环境
* @author 李章梅
* @create 2014/7/13
*/
class AmqpRpcContext : public AbstructRpcContext
{
public:
	AmqpRpcContext(){}
	virtual ~AmqpRpcContext(){}
};


}//end of namespace bluemei