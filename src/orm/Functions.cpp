#pragma once
#include "stdafx.h"
#include "Functions.h"

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

ConditionWrapper SqlFunctions::count(const Colume& col)
{
	return count(col.query()->toSQL());
}

ConditionWrapper SqlFunctions::sum(cstring col)
{
	return funcWithColume("sum", col);
}

ConditionWrapper SqlFunctions::sum(const Colume& col)
{
	return sum(col.query()->toSQL());
}

ConditionWrapper SqlFunctions::avg(cstring col)
{
	return funcWithColume("avg", col);
}

ConditionWrapper SqlFunctions::avg(const Colume& col)
{
	return avg(col.query()->toSQL());
}

ConditionWrapper SqlFunctions::min(cstring col)
{
	return funcWithColume("min", col);
}

ConditionWrapper SqlFunctions::min(const Colume& col)
{
	return min(col.query()->toSQL());
}

ConditionWrapper SqlFunctions::max(cstring col)
{
	return funcWithColume("max", col);
}

ConditionWrapper SqlFunctions::max(const Colume& col)
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