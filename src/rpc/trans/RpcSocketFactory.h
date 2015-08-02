#pragma once
#include "RpcSocket.h"


namespace brpc{

/*
* RPC socket π§≥ß
* @author ¿Ó’¬√∑
* @create 2012/5/28
*/
class RpcSocketFactory : public Object
{
public:
	static RpcSocket* getRpcSocket(const String& name);
};



}//end of namespace bluemei