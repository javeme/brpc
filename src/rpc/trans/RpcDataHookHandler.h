#pragma once
#include "blib.h"
#include "DataPackage.h"

namespace brpc{

class RpcReceiveListener;

/*
* RPC接收数据收发监听钩子
* @author Javeme
* @create 2012/8/8
*/
class RpcDataHookHandler : public virtual Object
{
public:
	virtual void onReceived(cstring name,const DataPackage& data,long time)=0;
	virtual void onSent(cstring name,const DataPackage& data,long time)=0;
	virtual void onError(cstring name,cstring err,RpcReceiveListener* listener)=0;
};

#define HOOK_ERR_RECV_STOPED "stoped-receiving"
#define HOOK_ERR_CLOSED "closed"

}//end of namespace brpc