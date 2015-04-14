#include "stdafx.h"
#include "RpcException.h"

namespace bluemei{


/*****************************************************************/
RpcException::RpcException(void):Exception("RPC Exception")
{
}

RpcException::RpcException(cstring str):Exception(str)
{
}

RpcException::~RpcException(void)
{
}

String RpcException::name() const
{
	return "RpcExceptions";
}



/*****************************************************************/
RpcCallException::RpcCallException( void )
{
}

RpcCallException::RpcCallException(int error, cstring str) : RpcException(str)
{
	this->err = error;
}

RpcCallException::~RpcCallException( void )
{
}

String RpcCallException::name() const
{
	return "RpcCallException";
}

int RpcCallException::error() const
{
	return this->err;
}

}//end of namespace bluemei