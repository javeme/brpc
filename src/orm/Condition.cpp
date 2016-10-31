#pragma once
#include "orm/Condition.h"


namespace brpc{

const ConditionWrapper ConditionWrapper::NONE = new ValueCondition("<null>");


// Condition operator string
ConditionWrapper Condition::operator==(cstring val)
{
	if (val == null) {
		return new IsCondition(this, new NullCondition());
	}
	else {
		return new EqCondition(this, new StringCondition(val));
	}
}

ConditionWrapper Condition::operator!=(cstring val)
{
	if (val == null) {
		return new IsCondition(this, new NotCondition(new NullCondition()));
	}
	else {
		return new NeCondition(this, new StringCondition(val));
	}
}

// like
ConditionWrapper Condition::like(cstring val)
{
	return new LikeCondition(this, new StringCondition(val));
}

// in(string)
ConditionWrapper Condition::in(cstring* vals, int size)
{
	ListCondition* list = new ListCondition();
	for(int i = 0; i < size; i++) {
		list->append(new StringCondition(vals[i]));
	}
	return new InCondition(this, list);
}

// between(string)
ConditionWrapper Condition::between(cstring from, cstring to)
{
	return new BetweenCondition(this,
		new StringCondition(from),
		new StringCondition(to));
}

// Condition operator int
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

// in(int)
ConditionWrapper Condition::in(int* vals, int size)
{
	ListCondition* list = new ListCondition();
	for(int i = 0; i < size; i++) {
		list->append(new IntCondition(vals[i]));
	}
	return new InCondition(this, list);
}

// between(int)
ConditionWrapper Condition::between(int from, int to)
{
	return new BetweenCondition(this,
		new IntCondition(from),
		new IntCondition(to));
}

// Condition operator double
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

// in(double)
ConditionWrapper Condition::in(double* vals, int size)
{
	ListCondition* list = new ListCondition();
	for(int i = 0; i < size; i++) {
		list->append(new DoubleCondition(vals[i]));
	}
	return new InCondition(this, list);
}

// between(double)
ConditionWrapper Condition::between(double from, double to)
{
	return new BetweenCondition(this,
		new DoubleCondition(from),
		new DoubleCondition(to));
}

// Condition operator ConditionWrapper
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

// and
ConditionWrapper Condition::operator&&(const ConditionWrapper& other)
{
	return new AndCondition(this, other);
}

// or
ConditionWrapper Condition::operator||(const ConditionWrapper& other)
{
	return new OrCondition(this, other);
}

// not
ConditionWrapper Condition::operator!()
{
	return new NotCondition(this);
}

}//end of namespace brpc