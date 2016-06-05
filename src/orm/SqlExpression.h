#pragma once
#include "blib.h"
#include "RpcException.h"


namespace brpc{

class SqlExpression : public Object
{
public:
	virtual String toString() const { return toSQL(); }
	virtual String toSQL() const = 0;
};


class SqlException : public RpcException
{
public:
	SqlException(void) {}
	SqlException(cstring str) : RpcException(str) {}
	virtual ~SqlException(void) {}
	String name() const {
		return "SqlException";
	}
};


}//end of namespace brpc