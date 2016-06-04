#pragma once
#include "RpcSocket.h"


namespace brpc{

/*
* RPC socket ����
* @author Javeme
* @create 2012/5/28
*/
class RpcSocketFactory : public Object
{
public:
	static RpcSocket* getRpcSocket(const String& name);
};



}//end of namespace brpc