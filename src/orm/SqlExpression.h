#pragma once
#include "blib/Ptr.h"
#include "src/util/RpcException.h"

#ifndef DEBUG_SQL
#define DEBUG_SQL 0
#endif


namespace brpc{

//class SqlExpression
class SQLExpression : public PointerReference
{
public:
	SQLExpression() {}
	virtual ~SQLExpression() {}

	SQLExpression(SQLExpression&& other) {
		*this = std::move(other);
	}

	SQLExpression& operator=(SQLExpression&& other) {
		PointerReference::operator=(std::move(other));
		return *this;
	}

private:
	explicit SQLExpression(const SQLExpression&);
	SQLExpression&  operator=(const SQLExpression&);

public:
	virtual String toString() const { return toSQL(); }
	virtual String toSQL() const = 0;

public:
	static int debug(cstring frmt, ...);
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