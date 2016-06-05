#pragma once
#include "stdafx.h"
#include "Condition.h"

namespace brpc{

const ConditionWrapper ConditionWrapper::NONE = new ValueCondition("<null>");


// Condition ? string
ConditionWrapper Condition::operator==(cstring val)
{
	return new EqCondition(this, new StringCondition(val));
}

ConditionWrapper Condition::operator!=(cstring val)
{
	return new NeCondition(this, new StringCondition(val));
}

// Condition ? int
ConditionWrapper Condition::operator==(int val)
{
	return new EqCondition(this, new IntCondition(val));
}

ConditionWrapper Condition::operator!=(int val)
{
	return new NeCondition(this, new IntCondition(val));
}

ConditionWrapper Condition::operator>(int val)
{
	return new GtCondition(this, new IntCondition(val));
}

ConditionWrapper Condition::operator>=(int val)
{
	return new GeCondition(this, new IntCondition(val));
}

ConditionWrapper Condition::operator<(int val)
{
	return new LtCondition(this, new IntCondition(val));
}

ConditionWrapper Condition::operator<=(int val)
{
	return new LeCondition(this, new IntCondition(val));
}

// Condition ? double
ConditionWrapper Condition::operator==(double val)
{
	return new EqCondition(this, new DoubleCondition(val));
}

ConditionWrapper Condition::operator!=(double val)
{
	return new NeCondition(this, new DoubleCondition(val));
}

ConditionWrapper Condition::operator>(double val)
{
	return new GtCondition(this, new DoubleCondition(val));
}

ConditionWrapper Condition::operator>=(double val)
{
	return new GeCondition(this, new DoubleCondition(val));
}

ConditionWrapper Condition::operator<(double val)
{
	return new LtCondition(this, new DoubleCondition(val));
}

ConditionWrapper Condition::operator<=(double val)
{
	return new LeCondition(this, new DoubleCondition(val));
}

// Condition ? ConditionWrapper
ConditionWrapper Condition::operator==(const ConditionWrapper& val)
{
	return new EqCondition(this, val);
}

ConditionWrapper Condition::operator!=(const ConditionWrapper& val)
{
	return new NeCondition(this, val);
}

ConditionWrapper Condition::operator>(const ConditionWrapper& val)
{
	return new GtCondition(this, val);
}

ConditionWrapper Condition::operator>=(const ConditionWrapper& val)
{
	return new GeCondition(this, val);
}

ConditionWrapper Condition::operator<(const ConditionWrapper& val)
{
	return new LtCondition(this, val);
}

ConditionWrapper Condition::operator<=(const ConditionWrapper& val)
{
	return new LeCondition(this, val);
}

// &&
ConditionWrapper Condition::operator&&(const ConditionWrapper& other)
{
	return new AndCondition(this, other);
}

// ||
ConditionWrapper Condition::operator||(const ConditionWrapper& other)
{
	return new OrCondition(this, other);
}

}//end of namespace brpc