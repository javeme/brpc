#pragma once
#include "blib.h"
#include "SqlExpression.h"


namespace brpc{

class ConditionWrapper;


class Condition : public SqlExpression, public PointerReference
{
public:
	Condition(){
		//printf(">>>>>>>>>>>>>>>Condition Init!!!!!!\n");
	}
	virtual ~Condition(){
		//printf(">>>>>>>>>>>>>>>Condition Release!!!\n");
	}
public:
	virtual ConditionWrapper operator ==(cstring val);
	virtual ConditionWrapper operator !=(cstring val);

	virtual ConditionWrapper operator ==(int val);
	virtual ConditionWrapper operator !=(int val);
	virtual ConditionWrapper operator >(int val);
	virtual ConditionWrapper operator >=(int val);
	virtual ConditionWrapper operator <(int val);
	virtual ConditionWrapper operator <=(int val);

	virtual ConditionWrapper operator ==(double val);
	virtual ConditionWrapper operator !=(double val);
	virtual ConditionWrapper operator >(double val);
	virtual ConditionWrapper operator >=(double val);
	virtual ConditionWrapper operator <(double val);
	virtual ConditionWrapper operator <=(double val);

	virtual ConditionWrapper operator ==(const ConditionWrapper& val);
	virtual ConditionWrapper operator !=(const ConditionWrapper& val);
	virtual ConditionWrapper operator >(const ConditionWrapper& val);
	virtual ConditionWrapper operator >=(const ConditionWrapper& val);
	virtual ConditionWrapper operator <(const ConditionWrapper& val);
	virtual ConditionWrapper operator <=(const ConditionWrapper& val);

	virtual ConditionWrapper operator &&(const ConditionWrapper& other);
	virtual ConditionWrapper operator ||(const ConditionWrapper& other);
};

class ConditionWrapper : public RefPointer<Condition>
{
public:
	ConditionWrapper(Condition* cond) : RefPointer(cond) {}

	/*ConditionWrapper(const ConditionWrapper& other) : m_target(other.m_target) {
		checkNullPtr(m_target);
		//printf(">>>>>>>>>>>>>>>Condition CCCCCCCCCCCCCCCCCCCopy!!!\n");
		m_target->attach();
	}

	virtual ~ConditionWrapper() {
		m_target->disattach();
	}

	ConditionWrapper& operator=(const ConditionWrapper& other) {
		m_target->disattach();
		m_target = other.m_target;
		m_target->attach();
		return *this;
	}*/
public:
	operator Condition& () const {
		return *(Condition*)*this;
	}
	
	virtual bool equals(const ConditionWrapper& other) const {
		return (Condition*)*this == (Condition*)other;
	}
public:
	virtual ConditionWrapper operator ==(cstring val) { return *m_target == val; }
	virtual ConditionWrapper operator !=(cstring val) { return *m_target != val; }

	virtual ConditionWrapper operator ==(int val) { return *m_target == val; }
	virtual ConditionWrapper operator !=(int val) { return *m_target != val; }
	virtual ConditionWrapper operator >(int val) { return *m_target > val; }
	virtual ConditionWrapper operator >=(int val) { return *m_target >= val; }
	virtual ConditionWrapper operator <(int val) { return *m_target < val; }
	virtual ConditionWrapper operator <=(int val) { return *m_target <= val; }

	virtual ConditionWrapper operator ==(double val) { return *m_target == val; }
	virtual ConditionWrapper operator !=(double val) { return *m_target != val; }
	virtual ConditionWrapper operator >(double val) { return *m_target > val; }
	virtual ConditionWrapper operator >=(double val) { return *m_target >= val; }
	virtual ConditionWrapper operator <(double val) { return *m_target < val; }
	virtual ConditionWrapper operator <=(double val) { return *m_target <= val; }

	virtual ConditionWrapper operator ==(Condition* val) { return *m_target == val; }
	virtual ConditionWrapper operator !=(Condition* val) { return *m_target != val; }
	virtual ConditionWrapper operator >(Condition* val) { return *m_target > val; }
	virtual ConditionWrapper operator >=(Condition* val) { return *m_target >= val; }
	virtual ConditionWrapper operator <(Condition* val) { return *m_target < val; }
	virtual ConditionWrapper operator <=(Condition* val) { return *m_target <= val; }

	virtual ConditionWrapper operator &&(const ConditionWrapper& other) {
		return *m_target && other;
	}
	virtual ConditionWrapper operator ||(const ConditionWrapper& other) {
		return *m_target || other;
	}
private:
	//PointerReference<Condition> m_target;
public:
	const static ConditionWrapper NONE;
};
#define CW_NONE ConditionWrapper::NONE


class FieldCondition : public Condition
{
public:
	FieldCondition(cstring name) : m_fieldName(name) {}
	virtual String toSQL() const {
		// TODO: escape(m_fieldName)
		return String::format("`%s`", m_fieldName.c_str());
	}
private:
	String m_fieldName;
};

class StringCondition : public Condition
{
public:
	StringCondition(cstring val) : m_str(val) {}
	virtual String toSQL() const {
		// TODO: escape(m_str)
		return String::format("'%s'", m_str.c_str());
	}
private:
	String m_str;
};

class ValueCondition : public Condition
{
public:
	ValueCondition(cstring val) : m_value(val) {}
	virtual String toSQL() const {
		// TODO: escape(m_value)
		return String::format("%s", m_value.c_str());
	}
private:
	String m_value;
};

class IntCondition : public Condition
{
public:
	IntCondition(int val) : m_val(val) {}
	virtual String toSQL() const {
		return String::format("%d", m_val);
	}
private:
	int m_val;
};

class DoubleCondition : public Condition
{
public:
	DoubleCondition(double val) : m_val(val) {}
	virtual String toSQL() const {
		return String::format("%f", m_val);
	}
private:
	double m_val;
};

class OperatorCondition : public Condition
{
public:
	OperatorCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: m_left(left),  m_right(right){
			//checkNullPtr(m_left);
			//checkNullPtr(m_right);
	}
	virtual ~OperatorCondition() {
		//delete m_left;
		//delete m_right;
	}

	virtual String toSQL() const {
		return String::format("(%s %s %s)",
			m_left->toSQL().c_str(),
			this->conditionOperator().c_str(),
			m_right->toSQL().c_str());
	}
	virtual String conditionOperator() const = 0;
protected:
	ConditionWrapper m_left;
	ConditionWrapper m_right;
};

class EqCondition : public OperatorCondition
{
public:
	EqCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "=";
	}
};

class NeCondition : public OperatorCondition
{
public:
	NeCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "!=";
	}
};

class GtCondition : public OperatorCondition
{
public:
	GtCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return ">";
	}
};

class GeCondition : public OperatorCondition
{
public:
	GeCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return ">=";
	}
};

class LtCondition : public OperatorCondition
{
public:
	LtCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "<";
	}
};

class LeCondition : public OperatorCondition
{
public:
	LeCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "<=";
	}
};

class AndCondition : public OperatorCondition
{
public:
	AndCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "and";
	}
};

class OrCondition : public OperatorCondition
{
public:
	OrCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "or";
	}
};


}//end of namespace brpc