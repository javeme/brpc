#pragma once
#include "stdafx.h"
#include "Colume.h"

namespace brpc{
	

Condition* Condition::operator==(cstring val)
{
	return new EqCondition(this, new StringCondition(val));
}

Condition* Condition::operator==(int val)
{
	return new EqCondition(this, new IntCondition(val));
}

Condition* Condition::operator>(int val)
{
	return new GtCondition(this, new IntCondition(val));
}

Condition* Condition::operator>=(int val)
{
	return new GeCondition(this, new IntCondition(val));
}

Condition* Condition::operator<(int val)
{
	return new LtCondition(this, new IntCondition(val));
}

Condition* Condition::operator<=(int val)
{
	return new LeCondition(this, new IntCondition(val));
}

Condition* Condition::operator==(double val)
{
	return new EqCondition(this, new DoubleCondition(val));
}

Condition* Condition::operator>(double val)
{
	return new GtCondition(this, new DoubleCondition(val));
}

Condition* Condition::operator>=(double val)
{
	return new GeCondition(this, new DoubleCondition(val));
}

Condition* Condition::operator<(double val)
{
	return new LtCondition(this, new DoubleCondition(val));
}

Condition* Condition::operator<=(double val)
{
	return new LeCondition(this, new DoubleCondition(val));
}

Condition* Condition::operator&&(Condition& other)
{
	return new AndCondition(this, &other);
}

Condition* Condition::operator||(Condition& other)
{
	return new OrCondition(this, &other);
}

}//end of namespace brpc