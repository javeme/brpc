#pragma once
#include "src/util/RpcException.h"

namespace brpc{

/*****************************************************************/
RpcException::RpcException(void) : blib::Exception("RPC Exception")
{
}

RpcException::RpcException(cstring str) : blib::Exception(str)
{
}

RpcException::RpcException(const blib::Exception& e) : blib::Exception(e)
{
}

RpcException::~RpcException(void)
{
}

String RpcException::name() const
{
	return "RpcException";
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

}//end of namespace brpc
