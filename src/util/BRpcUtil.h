#pragma once
#include "stdafx.h"
#include "CodeUtil.h"
#include "SmartPtr.h"
#include "Exceptions.h"
#include "StringBuilder.h"
#include "Number.h"
#include "TString.h"
#include "ObjectRef.h"

namespace bluemei{
	
/*
* 字符及其编码工具类
* @author 李章梅
* @create 2014/7/5
*/
class BRpcUtil : public CodeUtil
{
public:
	static int debug(cstring frmt, ...)
	{
		if(!isBrpcDebug())
			return -1;

		va_list arg_ptr;
		va_start(arg_ptr,frmt);

		return vprintf(frmt, arg_ptr);
	}

	static void setBrpcDebug(bool val) { s_isBrpcDebug = val; }
	static bool isBrpcDebug() { return s_isBrpcDebug; }

protected:
	static bool s_isBrpcDebug;
};


/*
* 类型不匹配异常类
* @author 李章梅
* @create 2014/7/4
*/
class TypeNotMatchedException : public BadCastException
{
public:
	TypeNotMatchedException(cstring msg) : BadCastException(msg)
	{
		;
	}

	TypeNotMatchedException(cstring val, cstring from, cstring to) 
		: BadCastException(String::format("%s(%s)", from, val), to)
	{
		;
	}

	virtual ~TypeNotMatchedException(void)
	{
	}

	String name()const
	{
		return "TypeNotMatchedException";
	}
};

/*
* 参数不匹配异常类
* @author 李章梅
* @create 2014/7/4
*/
class ArgNotMatchedException : public RuntimeException
{
public:
	ArgNotMatchedException(cstring msg) : RuntimeException(msg)
	{
		;
	}

	virtual ~ArgNotMatchedException(void)
	{
	}

	String name()const
	{
		return "ArgNotMatchedException";
	}
};

class AmbiguityFunctionException : public ArgNotMatchedException
{
public:
	AmbiguityFunctionException(cstring msg) 
		: ArgNotMatchedException(msg)
	{
		;
	}

	virtual ~AmbiguityFunctionException(void)
	{
	}

	String name() const
	{
		return "AmbiguityFunctionException";
	}
};

class NotMapException : public RuntimeException
{
public:
	NotMapException(Object* obj);
	virtual String name()const;
};

//Object <==> ObjectMap相互转换
struct MapObjectConverter
{
	static Object* object2map(Object* obj);
	static Object* map2object(Object* map, Object* obj);
};


template <typename T>
struct Ptr2Type;

template <typename T>
struct Ptr2Type<T*>
{
	typedef T Type;
};

template<typename Type, typename bool>
struct MapObjectHelper;

template<typename Type>
struct MapObjectHelper<Type, true>
{
	static Object* object2map(Type obj)
	{
		return MapObjectConverter::object2map(obj);
	}
	static Type map2object(Object* map)
	{
		typedef Ptr2Type<Type>::Type Cls;
		//cstring cls = Cls::thisClass()->getName();
		Object* obj = Cls::thisClass()->createObject();
		return dynamic_cast<Type>(MapObjectConverter::map2object(map, obj));
	}
};

template<typename Type>
struct MapObjectHelper<Type, false>
{
	static Object* object2map(Type)
	{
		return throwConversionException<Type, Object*>();
	}
	static Type map2object(Object* map)
	{
		return throwConversionException<Object*, Type>();
	}
};

/*
* Object对象转换为指定类型(无法转换时抛出异常)
* @author 李章梅
* @create 2014/7/4
*/
template <typename Type>
inline Type valueOf(Object* obj)
{
	checkNullPtr(obj);
	ObjectRef* ref = dynamic_cast<ObjectRef*>(obj);
	if (ref != null)
	{
		Object* originalObj = ref->getObject();
		return valueOf<Type>(originalObj);
	}

	try{
		return Converter<Type>::valueOf(obj);
	}catch(BadCastException&){
		//Type is a sub-Object
		const bool isSubObject = is_convertible<Type, Object*>::value;
		bool needMap2Object = false;
		if (isSubObject) {
			try{
				needMap2Object = true;
				return MapObjectHelper<Type, isSubObject>::map2object(obj);
			}catch(NotMapException&){
				needMap2Object = false;
			}
		}
		//needMap2Object must be false
		assert(!needMap2Object);
		//rethrow while there is no need to conv map to object
		throw;
	}
}

/*
* 指定类型转换为Object对象(无法转换时抛出异常)
* @author 李章梅
* @create 2014/7/4
*/
template <typename Type>
inline Object* toObject(const Type& val)
{
	return Converter<Type>::toObject(val);
}

template <typename Type>
inline Object* objectToMap(Type obj)
{
	//Type is a sub-Object
	const bool isSubObject = is_convertible<Type, Object*>::value;
	return MapObjectHelper<Type, isSubObject>::object2map(obj);
}

//转换到指定类型并检查是否成功
template <typename Type>
inline Type castAndCheck(Object* obj, cstring toCastType)
{
	checkNullPtr(obj);
	Type valObj = dynamic_cast<Type>(obj);
	if (valObj == null)
	{
		cstring type = obj->getThisClass()->getName();
		throwpe(TypeNotMatchedException(obj->toString(), type, toCastType))
	}
	return valObj;
}

inline Number* cast2noAndCheck(Object* obj, cstring toCastType, int pred)
{
	Number* number = castAndCheck<Number*>(obj, toCastType);
	if (number == null || number->precedence() > pred)
	{
		cstring type = obj->getThisClass()->getName();
		throwpe(TypeNotMatchedException(obj->toString(), type, toCastType));
	}
	return number;
}


//类型转换器(Object* <==> void)
template <>
struct Converter<void>
{
	static inline void valueOf(Object* obj)
	{		
		return;
	}
	static inline Object* toObject(void)
	{		
		return null;
	}
};

//类型转换器(Object* <==> bool)
template <>
struct Converter<bool>
{
	static inline bool valueOf(Object* obj)
	{		
		Boolean* b = castAndCheck<Boolean*>(obj, "boolean");
		return *b;
	}
	static inline Object* toObject(const bool& val)
	{		
		return new Boolean(val);
	}
};

//类型转换器(Object* <==> char)
template <>
struct Converter<char>
{
	static inline char valueOf(Object* obj)
	{		
		Char* ch = castAndCheck<Char*>(obj, "char");
		return *ch;
	}
	static inline Object* toObject(const char& val)
	{		
		return new Char(val);
	}
};

//类型转换器(Object* <==> short)
template <>
struct Converter<short>
{
	static inline short valueOf(Object* obj)
	{		
		Number* number = cast2noAndCheck(obj, "short", PRED_SHORT);
		return (short)number->toInt();
	}
	static inline Object* toObject(const short& val)
	{		
		return new Short(val);
	}
};

//类型转换器(Object* <==> int)
template <>
struct Converter<int>
{
	static inline int valueOf(Object* obj)
	{
		Number* number = cast2noAndCheck(obj, "int", PRED_INT);
		return number->toInt();
	}
	static inline Object* toObject(const int& val)
	{		
		return new Integer(val);
	}
};

//类型转换器(Object* <==> lint)
template <>
struct Converter<lint>
{
	static inline lint valueOf(Object* obj)
	{
		Number* number = cast2noAndCheck(obj, "long", PRED_INT64);
		return number->toLong();
	}
	static inline Object* toObject(const lint& val)
	{		
		return new Long(val);
	}
};

//类型转换器(Object* <==> float)
template <>
struct Converter<float>
{
	static inline float valueOf(Object* obj)
	{
		Number* number = cast2noAndCheck(obj, "float", PRED_FLOAT);
		return number->toFloat();
	}
	static inline Object* toObject(const float& val)
	{		
		return new Float(val);
	}
};

//类型转换器(Object* <==> double)
template <>
struct Converter<double>
{
	static inline double valueOf(Object* obj)
	{
		Number* number = cast2noAndCheck(obj, "double", PRED_DOUBLE);
		return number->toDouble();
	}
	static inline Object* toObject(const double& val)
	{		
		return new Double(val);
	}
};

template <>
struct Converter<Double>
{
	static inline Double valueOf(Object* obj)
	{
		Number* number = cast2noAndCheck(obj, "double", PRED_DOUBLE);
		return number->toDouble();
	}
	static inline Object* toObject(const Double& val)
	{		
		return new Double(val);
	}
};

//类型转换器(Object* <==> cstring)
template <>
struct Converter<cstring>
{
	static inline cstring valueOf(Object* obj)
	{
		String* str = castAndCheck<String*>(obj, "string");
		return str->c_str();
	}
	static inline Object* toObject(const cstring& val)
	{		
		return new TString(val);
	}
};

//类型转换器(Object* <==> char*)
typedef char* chars;
template <>
struct Converter<chars>
{
	static inline chars valueOf(Object* obj)
	{
		return (chars)Converter<cstring>::valueOf(obj);
	}
	static inline Object* toObject(const chars& val)
	{		
		cstring str = val;
		return Converter<cstring>::toObject(str);
	}
};

//类型转换器(Object* <==> std::string)
template <>
struct Converter<std::string>
{
	static inline std::string valueOf(Object* obj)
	{
		return Converter<cstring>::valueOf(obj);
	}
	static inline Object* toObject(const std::string& val)
	{		
		return Converter<cstring>::toObject(val.c_str());
	}
};

//类型转换器(Object* <==> String)
template <>
struct Converter<String>
{
	static inline String valueOf(Object* obj)
	{
		return Converter<cstring>::valueOf(obj);
	}
	static inline Object* toObject(const String& val)
	{		
		return Converter<cstring>::toObject(val.c_str());
	}
};


/*
template <>
struct Converter<Object>
{
	static inline Object* toObject(const Object& val)
	{
		return val.clone();
	}
};

template <>
struct Converter<Object*>
{
	static inline Object* toObject(const Object*& val)
	{
		return val->clone();
	}
};*/

template <typename T>
struct Converter<SmartPtr<T>>
{
	static inline SmartPtr<T> valueOf(Object* obj)
	{	
		return obj;
	}

	static inline Object* toObject(const SmartPtr<T>& val)
	{
		return val;
	}
};

//...


enum MatchLevel{ LEVEL_NOT_MATCHED, LEVEL_CAN_CONVERT, LEVEL_MATCHED };
template <typename Type>
inline MatchLevel matchLevel(Object* obj)
{
	try{
		Type val = valueOf<Type>(obj);
		
		Number* number = dynamic_cast<Number*>(obj);
		if (number != null)
		{
			SmartPtr<Object> objBack = toObject(val);
			Number* numberBack = dynamic_cast<Number*>((Object*)objBack);
			if (numberBack != null && numberBack->isSameType(*number)){
				return LEVEL_MATCHED;
			}
			return LEVEL_CAN_CONVERT;
		}
		else
			return LEVEL_MATCHED;
	}catch (Exception&)
	{
		return LEVEL_NOT_MATCHED;
	}
}


}//end of namespace bluemei