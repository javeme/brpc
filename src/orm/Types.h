#pragma once
#include "src/type/TypeConverter.h"


namespace brpc{

/////////////////////////////////////////////////////////////////
//dbtype
class dbtype : public Object
{
public:
	dbtype(cstring name) : m_name(name) {}
public:
	virtual cstring name() const { return m_name; }
	virtual cstring value() const = 0;
	virtual size_t length() const = 0;
private:
	String m_name;
};

/////////////////////////////////////////////////////////////////
//varchar
template <int SIZE=32>
class varchar : public dbtype
{
private:
	char chars[SIZE+1];

public:
	varchar(cstring str="") : dbtype(String::format("varchar(%d)", SIZE)) {
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


/////////////////////////////////////////////////////////////
// string_caster
template <typename Type, typename bool>
struct typename_getter;

template <typename Type>
struct typename_getter<Type, false>
{
	static cstring typeName(const Type& v) {
		// TODO: mapping all kinds of types into database types
		return typeid(Type).name();
	}
};

template <typename Type>
struct typename_getter<Type, true>
{
	static cstring typeName(const dbtype& v) {
		return v.name();
	}
};


}//end of namespace brpc


namespace blib{

//类型转换器(Object* <==> varchar<int>)
#define DEFINE_VARCHAR_CONVERTER(len)                                         \
template <>                                                                   \
struct Converter<brpc::varchar<len>>                                          \
{                                                                             \
    static inline brpc::varchar<len> valueOf(Object* obj)                     \
    {                                                                         \
        return Converter<cstring>::valueOf(obj);                              \
    }                                                                         \
    static inline Object* toObject(const brpc::varchar<len>& val)             \
    {                                                                         \
        return Converter<cstring>::toObject(val.value());                     \
    }                                                                         \
};                                                                            \
//end of DEFINE_VARCHAR_CONVERTER

DEFINE_VARCHAR_CONVERTER(16);
DEFINE_VARCHAR_CONVERTER(32);
DEFINE_VARCHAR_CONVERTER(64);
DEFINE_VARCHAR_CONVERTER(128);
DEFINE_VARCHAR_CONVERTER(256);

/*
//类型转换器(Object* <==> varchar<int>)
template <>
struct Converter<brpc::varchar<int>>
{
	static inline brpc::varchar<int> valueOf(Object* obj)
	{
		return Converter<cstring>::valueOf(obj);
	}
	static inline Object* toObject(const brpc::varchar<int>& val)
	{
		return Converter<varchar<int>>::toObject(val.value());
	}
};
*/

//template<> Value2String<brpc::varchar<32>>::operator String() const{ return (cstring)value; }

}//end of namespace blib