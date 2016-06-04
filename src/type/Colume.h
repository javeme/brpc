#pragma once
#include "blib.h"


namespace brpc{


class Condition : public Object
{
public:
	Condition* operator ==(cstring val);

	Condition* operator ==(int val);
	Condition* operator >(int val);
	Condition* operator >=(int val);
	Condition* operator <(int val);
	Condition* operator <=(int val);

	Condition* operator ==(double val);
	Condition* operator >(double val);
	Condition* operator >=(double val);
	Condition* operator <(double val);
	Condition* operator <=(double val);

	Condition* operator &&(Condition& other);
	Condition* operator ||(Condition& other);
public:
	virtual String toString() const { return toSQL(); }
	virtual String toSQL() const = 0;
};

class FieldCondition : public Condition
{
public:
	FieldCondition(cstring name) : m_fieldName(name) {}
	virtual String toSQL() const {
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
		return String::format("'%s'", m_str.c_str());
	}
private:
	String m_str;
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
	OperatorCondition(Condition* left, Condition* right)
		: m_left(left),  m_right(right){
			checkNullPtr(m_left);
			checkNullPtr(m_right);
	}
	virtual ~OperatorCondition() {
		delete m_left;
		delete m_right;
	}

	virtual String toSQL() const {
		return String::format("(%s %s %s)",
			m_left->toSQL().c_str(),
			this->conditionOperator().c_str(),
			m_right->toSQL().c_str());
	}
	virtual String conditionOperator() const = 0;
protected:
	Condition* m_left;
	Condition* m_right;
};

class EqCondition : public OperatorCondition
{
public:
	EqCondition(Condition* left, Condition* right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "=";
	}
};

class GtCondition : public OperatorCondition
{
public:
	GtCondition(Condition* left, Condition* right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return ">";
	}
};

class GeCondition : public OperatorCondition
{
public:
	GeCondition(Condition* left, Condition* right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return ">=";
	}
};

class LtCondition : public OperatorCondition
{
public:
	LtCondition(Condition* left, Condition* right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "<";
	}
};

class LeCondition : public OperatorCondition
{
public:
	LeCondition(Condition* left, Condition* right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "<=";
	}
};

class AndCondition : public OperatorCondition
{
public:
	AndCondition(Condition* left, Condition* right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "and";
	}
};

class OrCondition : public OperatorCondition
{
public:
	OrCondition(Condition* left, Condition* right)
		: OperatorCondition(left, right) {
	}

	virtual String conditionOperator() const {
		return "or";
	}
};

template <int SIZE=32>
struct varchar{
	char chars[SIZE+1];

	varchar(cstring str="") {
		memset(chars, 0, sizeof(chars));
		strncpy_s(chars, str, min(SIZE, strlen(str)));
	}
	varchar& operator=(cstring str) {
		strncpy_s(chars, str, min(SIZE, strlen(str)));
		return *this;
	}

	size_t size() const { return SIZE; }
	size_t length() const { return strlen(chars); }
	cstring value() const { return chars; }
	operator cstring() const { return value(); }
};


//field value with type info
template <typename T/*, T deftVal=T()*/>
class TypeField : public bluemei::Field<T>
{
public:
	TypeField() : m_fieldInfo(null) {}
	TypeField(const T& val) : Field<T>(val) {}
	virtual ~TypeField() {}
public:
	virtual TypeField& operator=(const T& val) {
		Field<T>::operator=(val);
		return *this;
	}

	const FieldInfo* getType() const { return m_fieldInfo; }
	void setType(const FieldInfo* val) { m_fieldInfo = val; }

public:
	Condition* query() const {
		return new FieldCondition(m_fieldInfo->name());
	}

private:
	const FieldInfo* m_fieldInfo;
};


//define a db colume(field)
#define COLUME(type, name) \
	TypeField<type> name; 												\
	class InerClassForRegField_##name									\
	{																	\
	public:																\
		InerClassForRegField_##name(){									\
		    static bool notReged = true;								\
			if(notReged) {												\
				regField(#name, &Self::name);							\
				notReged = false;										\
			}															\
			Self* parent = (Self*)((char*)this - offsetof(Self,			\
				_instanceOfInerClassForRegField_##name));				\
			parent->name.setType(Self::thisClass()->getField(#name));	\
		}																\
	}_instanceOfInerClassForRegField_##name;							\
/*end of COLUME*/


}//end of namespace brpc


namespace bluemei{

//ÀàÐÍ×ª»»Æ÷(Object* <==> TypeField)
template <typename Type>
struct Converter<brpc::TypeField<Type>>
{
	static inline brpc::TypeField<Type> valueOf(Object* obj)
	{		
		brpc::TypeField<Type>* fld = dynamic_cast<brpc::TypeField<Type>*>(obj);
		if(fld)
			return fld->value();
		return Converter<Type>::valueOf(obj);
	}
	static inline Object* toObject(const brpc::TypeField<Type>& val)
	{		
		return Converter<Type>::toObject(val.value());
	}
};

}//end of namespace bluemei