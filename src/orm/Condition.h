#pragma once
#include "blib.h"
#include "SqlExpression.h"


namespace brpc{

class ConditionWrapper;


/* class Condition
 * Base class of all values/conditions like a number / operator > expression
 * We may use virtual inheritance to PointerReference
 * For more simple we let SQLExpression inherit PointerReference
 */
class Condition : public SQLExpression /*, public PointerReference*/
{
public:
	Condition(){
		//debug(">>>>>>>>>>>>>>>Condition Init!!!!!!\n");
	}
	virtual ~Condition(){
		//debug(">>>>>>>>>>>>>>>Condition Release!!!\n");
	}

	//#pragma warning(disable:4250)
	//多继承中，菱形继承时不明确的方法继承(通过子类显示重载toString解决)
	virtual String toString()const{
		return SQLExpression::toString();
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
	virtual ConditionWrapper operator !();

	virtual ConditionWrapper like(cstring val);
	virtual ConditionWrapper in(cstring* vals, int size);
	virtual ConditionWrapper between(cstring from, cstring to);

	virtual ConditionWrapper in(int* vals, int size);
	virtual ConditionWrapper between(int from, int to);

	virtual ConditionWrapper in(double* vals, int size);
	virtual ConditionWrapper between(double from, double to);
};

class ConditionWrapper : public RefPointer<Condition>
{
public:
	ConditionWrapper() : RefPointer(null) {}
	ConditionWrapper(Condition* cond) : RefPointer(cond) {}

private:
	bool operator==(const ConditionWrapper& other) const {
		return (Condition*)*this == (Condition*)other;
	}

public:
	operator Condition& () const {
		return *(Condition*)*this;
	}
	
	virtual bool equals(const ConditionWrapper& other) const {
		return this->operator==(other);
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

	virtual ConditionWrapper operator ==(const ConditionWrapper& val) {
		return *m_target == val;
	}
	virtual ConditionWrapper operator !=(const ConditionWrapper& val) {
		return *m_target != val;
	}
	virtual ConditionWrapper operator >(const ConditionWrapper& val) {
		return *m_target > val;
	}
	virtual ConditionWrapper operator >=(const ConditionWrapper& val) {
		return *m_target >= val;
	}
	virtual ConditionWrapper operator <(const ConditionWrapper& val) {
		return *m_target < val;
	}
	virtual ConditionWrapper operator <=(const ConditionWrapper& val) {
		return *m_target <= val;
	}

	virtual ConditionWrapper operator &&(const ConditionWrapper& other) {
		return *m_target && other;
	}
	virtual ConditionWrapper operator ||(const ConditionWrapper& other) {
		return *m_target || other;
	}
	virtual ConditionWrapper operator !() {
		return !(*m_target);
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

class NullCondition : public ValueCondition
{
public:
	NullCondition() : ValueCondition("null") {}
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


class UnaryOperatorCondition : public Condition
{
public:
	UnaryOperatorCondition(const ConditionWrapper& val)
		: m_value(val) {
		//checkNullPtr(m_value);
	}
	virtual ~UnaryOperatorCondition() {
		//delete m_value;
	}

	virtual String toSQL() const {
		return String::format("%s %s",
			this->conditionOperator().c_str(),
			this->m_value->toSQL().c_str());
	}
	virtual String conditionOperator() const = 0;
protected:
	ConditionWrapper m_value;
};

class BinOperatorCondition : public Condition
{
public:
	BinOperatorCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: m_left(left),  m_right(right){
		//NOTE: allow it to be null
		//checkNullPtr(m_left);
		//checkNullPtr(m_right);
	}
	virtual ~BinOperatorCondition() {
		//NOTE: We use ref pointer, so don't delete these pointer anymore
		//delete m_left;
		//delete m_right;
	}

	virtual String toSQL() const {
		return String::format("(%s %s %s)",
			this->m_left->toSQL().c_str(),
			this->conditionOperator().c_str(),
			this->m_right->toSQL().c_str());
	}
	virtual String conditionOperator() const = 0;
protected:
	ConditionWrapper m_left;
	ConditionWrapper m_right;
};

class ListCondition : public Condition
{
public:
	ListCondition() {}

public:
	virtual void append(const ConditionWrapper& val) {
		m_vals.add(val);
	}

	virtual unsigned int size() const {
		return m_vals.size();
	}

	virtual String toSQL() const {
		StringBuilder sb(m_vals.size() * 32);
		sb.append("(");
		for(unsigned int i = 0; i < m_vals.size(); i++) {
			sb.append(m_vals[i]);
			sb.append(",");
		}
		//delete the last char
		if(m_vals.size() > 0)
			sb.pop();
		sb.append(")");
		return sb.toString();
	}

protected:
	ArrayList<ConditionWrapper> m_vals;
};

class EqCondition : public BinOperatorCondition
{
public:
	EqCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: BinOperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "=";
	}
};

class NeCondition : public BinOperatorCondition
{
public:
	NeCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: BinOperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "!=";
	}
};

class GtCondition : public BinOperatorCondition
{
public:
	GtCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: BinOperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return ">";
	}
};

class GeCondition : public BinOperatorCondition
{
public:
	GeCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: BinOperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return ">=";
	}
};

class LtCondition : public BinOperatorCondition
{
public:
	LtCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: BinOperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "<";
	}
};

class LeCondition : public BinOperatorCondition
{
public:
	LeCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: BinOperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "<=";
	}
};

class IsCondition : public BinOperatorCondition
{
public:
	IsCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: BinOperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "is";
	}
};

class LikeCondition : public BinOperatorCondition
{
public:
	LikeCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: BinOperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "like";
	}
};

class InCondition : public BinOperatorCondition
{
public:
	//NOTE: the right value is an instance of ListCondition
	InCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: BinOperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "in";
	}
};

class BetweenCondition : public Condition
{
public:
	BetweenCondition(const ConditionWrapper& val,
		const ConditionWrapper& from, const ConditionWrapper& to)
		: m_value(val), m_from(from), m_to(to){
	}

	virtual String toSQL() const {
		return String::format("%s between %s and %s",
			this->m_value->toSQL().c_str(),
			this->m_from->toSQL().c_str(),
			this->m_to->toSQL().c_str());
	}

protected:
	ConditionWrapper m_value;
	ConditionWrapper m_from;
	ConditionWrapper m_to;
};

class AndCondition : public BinOperatorCondition
{
public:
	AndCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: BinOperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "and";
	}
};

class OrCondition : public BinOperatorCondition
{
public:
	OrCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: BinOperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "or";
	}
};

class NotCondition : public UnaryOperatorCondition
{
public:
	NotCondition(const ConditionWrapper& val): UnaryOperatorCondition(val) {}

	virtual String conditionOperator() const {
		return "not";
	}
};


// AsCondition: actually this is not a condition!
class AsCondition : public BinOperatorCondition
{
public:
	AsCondition(const ConditionWrapper& left, const ConditionWrapper& right)
		: BinOperatorCondition(left, right) {
	}

	virtual String toSQL() const {
		return String::format("%s %s %s",
			this->m_left->toSQL().c_str(),
			this->conditionOperator().c_str(),
			this->m_right->toSQL().c_str());
	}

	virtual String conditionOperator() const {
		return "as";
	}
};


}//end of namespace brpc


namespace bluemei{

using brpc::ConditionWrapper;
using brpc::Condition;

template <>
struct dynamic_caster<ConditionWrapper, true>
{
	static ConditionWrapper toType(Object* obj){
		checkNullPtr(obj);
		if (dynamic_cast<Condition*>(obj))
			return dynamic_cast<Condition*>(obj);
		return throwConversionException<Object*, ConditionWrapper>();
	}
	static Object* toObject(ConditionWrapper val){
		if (val.equals(null))
			return null;
		else if (dynamic_cast<Object*>((Condition*)val))
			return dynamic_cast<Object*>((Condition*)val);
		return throwConversionException<ConditionWrapper, Object*>();
	}
};

}//end of namespace bluemei