#pragma once
#include "blib.h"
#include "RpcException.h"

#ifndef DEBUG_SQL
#define DEBUG_SQL 0
#endif

namespace brpc{


//class SqlExpression
class SQLExpression : public PointerReference
{
public:
	virtual String toString() const { return toSQL(); }
	virtual String toSQL() const = 0;

public:
	static int debug(cstring frmt, ...) {
		if(!DEBUG_SQL)
			return -1;

		va_list arg_ptr;
		va_start(arg_ptr, frmt);

		return vprintf(frmt, arg_ptr) + printf("\n");
	}
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