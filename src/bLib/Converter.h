#ifndef Converter_H_H
#define Converter_H_H
#include "bluemeiLib.h"
#include "Object.h"

namespace bluemei{
		
//字符串化
#define _T2STR(type) #type

//匿名变量
#define __ANONYMOUS(var, i) var##i
#define _ANONYMOUS(var, i) __ANONYMOUS(var, i)
#define ANONYMOUS(var) _ANONYMOUS(var, __LINE__)


BLUEMEILIB_API void throwBadCastException(cstring from, cstring to);
BLUEMEILIB_API void throwTypeException(const Object* instance, cstring cls);


template <typename From, typename To>
static To throwConversionException(){
	cstring from = typeid(From).name();
	cstring to = typeid(To).name();
	throwBadCastException(from, to);
	throw Exception("Conversion Exception");//no exeute
}

template <typename Type, typename bool>
struct static_caster;

template <typename Type>
struct static_caster<Type, true>
{
	static Object* toObject(Type val){
		return static_cast<Object*>(val);
	}
};

template <typename Type>
struct static_caster<Type, false>
{
	static Object* toObject(Type val){
		return throwConversionException<Type, Object*>();
	}
};


template <typename Type, typename bool>
struct dynamic_caster;

template <typename Type>
struct dynamic_caster<Type, true>
{
	static Type toType(Object* obj){
		checkNullPtr(obj);
		if (dynamic_cast<Type>(obj))
			return dynamic_cast<Type>(obj);
		return throwConversionException<Object*, Type>();
	}
	static Object* toObject(Type val){
		if (val == null)
			return null;
		else if (dynamic_cast<Object*>(val))
			return dynamic_cast<Object*>(val);
		return throwConversionException<Type, Object*>();
	}
};

typedef decltype(null) NullType;
template <>
struct dynamic_caster<NullType, true>
{
	static NullType toType(Object* obj)
	{		
		return null;
	}
	static Object* toObject(NullType val)
	{		
		return null;
	}
};

template <typename Type>
struct dynamic_caster<Type, false>
{
	static Type toType(Object* obj){
		return throwConversionException<Object*, Type>();
	}
	static Object* toObject(Type val){
		return throwConversionException<Type, Object*>();
	}
};

template <typename From, typename To>
struct is_convertible
{
	struct Yes{ char v; };
	struct No { short v; };
	static Yes canConvert(const To&);
	static No canConvert(...);
	
	//if int returns long: int() == 0 ==> Object*, but long() !=> Object*
	static long isIntFrom(int&);
	//else returns itself
	static From isIntFrom(...);

	//enum { value = std::is_same<canConvert(From()), Yes>::value; }
	enum { value = (sizeof(canConvert(isIntFrom(From()))) == sizeof(Yes)) };
};

template <>
struct is_convertible<void, void>
{
	enum { value = true };
};

template <typename To>
struct is_convertible<void, To>
{
	enum { value = false };
};

template <typename From>
struct is_convertible<From, void>
{
	enum { value = false };
};

//类型转换器
template <typename Type>
struct BLUEMEILIB_TEMPLATE Converter
{
	static inline Type valueOf(Object* obj)
	{
		/*if(dynamic_cast<Type>(obj)) //dynamic_cast<int>(obj):error
			return (Type)obj;
		*/
		//const bool convertible = std::is_convertible<Type, Object*>::value;
		const bool convertible = is_convertible<Type, Object*>::value;
		return dynamic_caster<Type, convertible>::toType(obj);
	}

	static inline Object* toObject(const Type& val)
	{
		const bool convertible = is_convertible<Type, Object*>::value;
		return dynamic_caster<Type, convertible>::toObject(val);
		//return null;
	}
};


/*test is_convertible
convertible = is_convertible<int,int>::value;
convertible = is_convertible<int,long>::value;
convertible = is_convertible<long,int>::value;
convertible = is_convertible<float,int>::value;
convertible = is_convertible<int,Object*>::value;
convertible = is_convertible<long,Object*>::value;
convertible = is_convertible<Object*,void*>::value;
convertible = is_convertible<void*,Object*>::value;
convertible = is_convertible<void,void>::value;
convertible = is_convertible<void,Object*>::value;
convertible = is_convertible<Object*,void>::value;
*/

}//end of namespace bluemei
#endif