#pragma once
#include "blib.h"
#include "Condition.h"
#include "BRpcUtil.h"


namespace brpc{

/////////////////////////////////////////////////////////////////
//varchar
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
	operator String() const { return value(); }
};


/////////////////////////////////////////////////////////////////
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

	virtual const FieldInfo* getType() const { return m_fieldInfo; }
	virtual void setType(const FieldInfo* val) { m_fieldInfo = val; }

public:
	cstring fieldName() const {
		return m_fieldInfo ? m_fieldInfo->name() : "";
	}

private:
	const FieldInfo* m_fieldInfo;
};


//Colume Model
class Colume : public Object
{
public:
	Colume(cstring name="", bool primary=false, bool notNull=false)
		: m_name(name), m_primary(primary), m_notNull(notNull) {}
public:
	virtual void setColumeName(cstring str) { m_name = str; }
	virtual String columeName() const { return m_name; }
public:
	virtual ConditionWrapper query() const = 0;
	virtual String columeValue() const = 0;
protected:
	String m_name;
	bool m_primary;
	bool m_notNull;
};

template <typename T>
class TypeColume : public Colume, public TypeField<T>
{
public:
	TypeColume() : Colume(), TypeField<T>() {}
	TypeColume(const T& val) : Colume(), TypeField<T>(val) {}
public:
	virtual TypeColume& operator=(const T& val) {
		TypeField<T>::operator=(val);
		return *this;
	}

	virtual String toString() const {
		return Value2String<T>((T)*this);
	}

public:
	virtual ConditionWrapper query() const {
		return new FieldCondition(this->columeName());
	}
	virtual String columeName() const {
		return m_name.length() ? m_name : fieldName();
	}
	virtual String columeValue() const {
		const bool convertible = is_convertible<T, String>::value;
		return string_caster<T, convertible>::toString(*this);
		//return Value2String<T>(*this);
	}
};


#define TABLE(table) \
	static cstring tableName() { return #table; }						\
	virtual String getTableName() const { return tableName(); }			\

//define a db colume(field)
#define COLUME3(name, type, colName) \
	TypeColume<type> name;												\
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
			parent->name.setColumeName(colName);						\
		}																\
	}_instanceOfInerClassForRegField_##name;							\
/*end of COLUME*/

#define COLUME2(name, type) COLUME3(name, type, "")
#define COLUME1(name) COLUME2(name, varchar<64>)
#define COLUME(name, type) COLUME2(name, type)

#define ID(type, colName) COLUME3(_id, type, colName); virtual Colume& id(){ return _id; }


}//end of namespace brpc


namespace bluemei{

//类型转换器(Object* <==> TypeField)
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

//类型转换器(Object* <==> TypeColume)
template <typename Type>
struct Converter<brpc::TypeColume<Type>>
{
	static inline brpc::TypeColume<Type> valueOf(Object* obj)
	{		
		brpc::TypeColume<Type>* fld = dynamic_cast<brpc::TypeColume<Type>*>(obj);
		if(fld)
			return fld->value();
		return Converter<Type>::valueOf(obj);
	}
	static inline Object* toObject(const brpc::TypeColume<Type>& val)
	{		
		return Converter<Type>::toObject(val.value());
	}
};

//template<> Value2String<brpc::varchar<32>>::operator String() const{ return (cstring)value; }

}//end of namespace bluemei