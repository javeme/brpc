#pragma once
#include "src/rpc/trans/RpcSocket.h"


namespace brpc{

/*
* RPC Socket Factory
* @author Javeme
* @create 2012/5/28
*/
class RpcSocketFactory : public Object
{
public:
	static RpcSocket* getRpcSocket(const String& name);
};

}//end of namespace brpc