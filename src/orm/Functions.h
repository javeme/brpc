#pragma once
#include "blib.h"
#include "Condition.h"
#include "Column.h"


namespace brpc{


class SqlFunctions : public Object
{
public:
	static ConditionWrapper count();
	static ConditionWrapper count(cstring col);
	static ConditionWrapper count(const Column& col);

	static ConditionWrapper sum(cstring col);
	static ConditionWrapper sum(const Column& col);

	static ConditionWrapper avg(cstring col);
	static ConditionWrapper avg(const Column& col);

	#undef min
	static ConditionWrapper min(cstring col);
	static ConditionWrapper min(const Column& col);

	#undef max
	static ConditionWrapper max(cstring col);
	static ConditionWrapper max(const Column& col);

	static ConditionWrapper date();
	static ConditionWrapper time();
	static ConditionWrapper datetime();
};


// func: likes avg(age) -- code, as: a string(field name)
#define AS(func, as) \
	AsCondition(SqlFunctions::func, new FieldCondition(as)).toSQL()

// register a filed that related to a function
#define FUNCTION(name, type, func) \
	COLUME8(name, type, AS(func, #name), false, false, false, "", false)

//function with filed name, func: ident, filed: a string
#define FUNCTION_F(func, filed) FUNCTION(func, int, func(filed))

//void function, func: ident
#define FUNCTION_V(func, type) FUNCTION(func, type, func())

//function with filed ident, func: ident, col: ident.
//TODO: parse the col type
#define FUNCTION_C(func, col) FUNCTION_F(func, parent.col)


}//end of namespace brpc