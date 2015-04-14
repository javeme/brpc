#pragma once
#include "Exception.h"

namespace bluemei{


/*
* Rpc“Ï≥£
* @author ¿Ó’¬√∑
* @create 2013/5/28
*/
class RpcException : public Exception
{
public:
	RpcException(void);
	RpcException(cstring str);
	virtual ~RpcException(void);
	String name() const;
};

class RpcCallException : public RpcException
{
public:
	RpcCallException(void);
	RpcCallException(int error, cstring str);
	virtual ~RpcCallException(void);
	String name() const;
	int error()const;
private:
	int err;
};


}//end of namespace bluemei