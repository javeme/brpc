#ifndef ClassField_H_H
#define ClassField_H_H
#include "bluemeiLib.h"
#include "Converter.h"
#include "Object.h"

namespace bluemei{

//for field in class
class BLUEMEILIB_API FieldInfo : public Object
{
public:
	typedef const type_info& TypeInfo;
	FieldInfo() {}
	virtual ~FieldInfo() {}
public:
	virtual cstring name() const = 0;
	virtual TypeInfo typeInfo() const = 0;

	virtual bool isPublic() const = 0;
	virtual void setPublic(bool val) = 0;

	virtual void setValue(Object& obj, Object* val) const = 0;
	virtual Object* getValue(const Object& obj) const = 0;

	virtual const void* getAddr(const Object& obj) const = 0;

	virtual FieldInfo* clone() const = 0;
public:
	virtual bool operator==(const FieldInfo& fld) const {
		return this == &fld;
	}
	virtual bool operator!=(const FieldInfo& fld) const {
		return !(*this == fld);
	}
};


//Common Field(not supported now)
template <typename Type>
class FieldType;

//Class Field
template <typename C, typename T>
class BLUEMEILIB_TEMPLATE FieldType<T(C::*)> : public FieldInfo
{
public:
	typedef C Cls;
	typedef T Var;
	typedef Var(Cls::*Offset);

	FieldType(const std::string& name, Offset offset, bool isPublic=true)
		: m_name(name), m_type(typeid(Var)), m_offset(offset), m_isPublic(isPublic) {}
	virtual ~FieldType() {}
public:
	virtual cstring name() const { return m_name.c_str(); }
	virtual TypeInfo typeInfo() const { return m_type; }

	virtual bool isPublic() const { return m_isPublic; }
	virtual void setPublic(bool val) { m_isPublic = val; }

	virtual void setValue(Object& obj, Object* val) const
	{
		Cls* inst = dynamic_cast<Cls*>(&obj);
		if (inst == null)
			throwTypeException(&obj, typeid(Cls*).name());
		return this->set(*inst, Converter<Var>::valueOf(val));
	}
	virtual Object* getValue(const Object& obj) const
	{
		const Cls* inst = dynamic_cast<const Cls*>(&obj);
		if (inst == null)
			throwTypeException(&obj, typeid(const Cls*).name());
		return Converter<Var>::toObject(this->get(*inst));
	}

	virtual const void* getAddr(const Object& obj) const
	{
		const Cls* inst = dynamic_cast<const Cls*>(&obj);
		if (inst == null)
			throwTypeException(&obj, typeid(const Cls*).name());
		return &this->get(*inst);
	}

	virtual FieldInfo* clone() const { return new FieldType(*this); }

	void set(Cls& obj, const Var& val) const { obj.*m_offset = val; }
	const Var& get(const Cls& obj) const { return obj.*m_offset; }
public:
	static bool registerField(const FieldType& fld)
	{
		return Cls::thisClass()->putField(fld);
	}
private:
	std::string m_name;
	TypeInfo m_type;
	bool m_isPublic;
	Offset m_offset;
};


//for field in class
class BLUEMEILIB_API FieldBase : public Object
{
public:
	virtual bool modified() const = 0;
	virtual void setModified(bool state) = 0;

	virtual bool setted() const = 0;
	virtual void setSetted(bool state) = 0;
};

//field value
template <typename T/*, T deftVal=T()*/>
class BLUEMEILIB_TEMPLATE Field : public FieldBase
{
public:
	Field() : /*m_value(deftVal),*/ m_setted(false), m_modified(false) {}
	Field(const T& val) : m_value(val), m_setted(true), m_modified(false) {}
	virtual ~Field() {}
public:
	virtual Field& operator=(const T& val) {
		m_value = val;
		m_setted = true;
		m_modified = true;
		return *this;
	}
public:
	T value() const { return m_value; }
	operator T() const { return value(); }
	//const Field& fieldType() const { return *m_fieldType; }

	virtual bool modified() const { return m_modified; }
	virtual void setModified(bool state) { m_modified = state; }

	virtual bool setted() const { return m_setted; }
	virtual void setSetted(bool state) { m_setted = state; }

	bool operator==(const T& v) const { return m_value == v; }
	bool operator!=(const T& v) const { return !(*this == v); }

	bool operator==(const Field& v) const { return m_value == v.m_value; }
	bool operator!=(const Field& v) const { return !(*this == v); }

private:
	T m_value;
	bool m_setted;
	bool m_modified;
	//T m_default;
};


//ÀàÐÍ×ª»»Æ÷(Object* <==> Field)
template <typename Type>
struct Converter<Field<Type>>
{
	static inline Field<Type> valueOf(Object* obj)
	{
		Field<Type>* fld = dynamic_cast<Field<Type>*>(obj);
		if(fld)
			return fld->value();
		return Converter<Type>::valueOf(obj);
	}
	static inline Object* toObject(const Field<Type>& val)
	{
		return Converter<Type>::toObject(val.value());
	}
};


template<typename Type>
struct BLUEMEILIB_TEMPLATE FieldRegister
{
public:
	FieldRegister::FieldRegister(cstring name, Type fieldType)
		: field(name, fieldType)
	{
		registerFieldCheck(FieldType<Type>::registerField(field), name);
	}
private:
	FieldType<Type> field;
};

//decltype => FieldType<Type>::Var(FieldType<Type>::Cls::*)
#define REG_CLS_FIELD(cls, fld) \
	static FieldRegister<decltype(&cls::fld)> ANONYMOUS(__s_regFor_##cls##_##fld)(_T2STR(fld), &cls::fld);

BLUEMEILIB_API void registerFieldCheck(bool success, cstring name);

template<typename Type>
static FieldType<Type> regField(cstring name, Type fieldType)
{
	FieldType<Type> field(name, fieldType);
	registerFieldCheck(FieldType<Type>::registerField(field), name);
	return field;
}

template<typename Type>
static FieldType<Type> regFieldWithoutNs(cstring name, Type fieldType)
{
	const char* pos = name;
	while(pos = strstr(pos, "::")) {
		pos += 2;
		name = pos;
	}
	return regField(name, fieldType);
}

#define REG_FIELD(fld) regFieldWithoutNs(_T2STR(fld), &fld)

//define a class field, and register it into Class
#define FIELD(type, name) \
	Field<type> name; 													\
	class InerClassForRegField_##name									\
	{																	\
	public:																\
		InerClassForRegField_##name(){									\
			static bool notReged = true;								\
			if(notReged) {												\
				regField(#name, &Self::name);							\
				notReged = false;										\
			}															\
		}																\
	}_instanceOfInerClassForRegField_##name;							\
/*end of define a field*/

/*
template <typename T, const char* name, T fld>
class RegField
{
public:
	RegField(){
		regField(name, fld);
	}
};*/

}
#endif