#pragma once
#include "src/type/ObjectList.h"
#include "src/rpc/RpcInvoker.h"


namespace brpc{

/*
* RPC远程调用环境
* @author Javeme
* @create 2014/7/13
*/
class RpcContext : public HashMap<String, Object*>
{
public:
	RpcContext(RpcInvoker& invoker):invoker(invoker){}
	virtual ~RpcContext(){}
public:
	RpcInvoker& getInvoker(){ return invoker; }
protected:
	RpcInvoker& invoker;
};

}//end of namespace brpc