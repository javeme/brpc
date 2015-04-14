#pragma once
#include "Object.h"
#include "RpcSocket.h"

namespace bluemei{

/*
* RPC接收数据监听器
* @author 李章梅
* @create 2012/5/28
*/
class RpcSocketManager : public Object
{
public:
	static RpcSocket* getRpcSocket(const String& name);
};



}//end of namespace bluemei