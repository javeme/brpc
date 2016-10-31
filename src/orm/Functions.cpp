#pragma once
#include "orm/Functions.h"


namespace brpc{

#define funcWithColume(func, col) \
	new ValueCondition(String::format("%s(%s)", func, col))

//////////////////////////////////////////////////////////////////
// SqlFunctions
ConditionWrapper SqlFunctions::count()
{
	// TODO: call each driver's function
	return new ValueCondition("count(*)");
}

ConditionWrapper SqlFunctions::count(cstring col)
{
	return funcWithColume("count", col);
}

ConditionWrapper SqlFunctions::count(const Column& col)
{
	return count(col.query()->toSQL());
}

ConditionWrapper SqlFunctions::sum(cstring col)
{
	return funcWithColume("sum", col);
}

ConditionWrapper SqlFunctions::sum(const Column& col)
{
	return sum(col.query()->toSQL());
}

ConditionWrapper SqlFunctions::avg(cstring col)
{
	return funcWithColume("avg", col);
}

ConditionWrapper SqlFunctions::avg(const Column& col)
{
	return avg(col.query()->toSQL());
}

ConditionWrapper SqlFunctions::min(cstring col)
{
	return funcWithColume("min", col);
}

ConditionWrapper SqlFunctions::min(const Column& col)
{
	return min(col.query()->toSQL());
}

ConditionWrapper SqlFunctions::max(cstring col)
{
	return funcWithColume("max", col);
}

ConditionWrapper SqlFunctions::max(const Column& col)
{
	return max(col.query()->toSQL());
}

ConditionWrapper SqlFunctions::date()
{
	return new ValueCondition("date()");
}

ConditionWrapper SqlFunctions::time()
{
	return new ValueCondition("time()");
}

ConditionWrapper SqlFunctions::datetime()
{
	return new ValueCondition("now()");
}

}//end of namespace brpc