#pragma once
#include "blib.h"
#include "RpcException.h"


namespace brpc{


//class SqlExpression: use virtual inheritance due to Condition
//#pragma warning(disable:4250) // 菱形继承时不明确的方法继承(通过子类显示重载toString解决)
class SQLExpression : public PointerReference
{
public:
	virtual String toString() const { return toSQL(); }
	virtual String toSQL() const = 0;
};


class SQLException : public RpcException
{
public:
	SQLException(void) {}
	SQLException(cstring str) : RpcException(str) {}
	virtual ~SQLException(void) {}
	String name() const {
		return "SqlException";
	}
};


}//end of namespace brpc