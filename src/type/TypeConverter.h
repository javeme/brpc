#pragma once
#include "blib.h"
#include "Number.h"
#include "TString.h"


namespace brpc{

//类型转换器模板
template <typename Type>
struct Converter
{
	static inline Type valueOf(bluemei::Object* obj)
	{
		return bluemei::Converter<Type>::valueOf(obj);
	}

	static inline bluemei::Object* toObject(const Type& val)
	{
		return bluemei::Converter<Type>::toObject(val);
	}
};

}// end of namespace brpc


namespace bluemei{

/*
* 类型不匹配异常类
* @author Javeme
* @create 2014/7/4
*/
class TypeNotMatchedException : public BadCastException
{
public:
	TypeNotMatchedException(cstring msg) : BadCastException(msg){}

	TypeNotMatchedException(cstring val, cstring from, cstring to)
		: BadCastException(String::format("%s(%s)", from, val), to){}

	virtual ~TypeNotMatchedException(void){}

	String name()const {
		return "TypeNotMatchedException";
	}
};


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

inline brpc::Number* cast2noAndCheck(Object* obj, cstring toCastType, int pred)
{
	brpc::Number* number = castAndCheck<brpc::Number*>(obj, toCastType);
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
		brpc::Boolean* b = castAndCheck<brpc::Boolean*>(obj, "boolean");
		return *b;
	}
	static inline Object* toObject(const bool& val)
	{
		return new brpc::Boolean(val);
	}
};

//类型转换器(Object* <==> char)
template <>
struct Converter<char>
{
	static inline char valueOf(Object* obj)
	{
		brpc::Char* ch = castAndCheck<brpc::Char*>(obj, "char");
		return *ch;
	}
	static inline Object* toObject(const char& val)
	{
		return new brpc::Char(val);
	}
};

//类型转换器(Object* <==> short)
template <>
struct Converter<short>
{
	static inline short valueOf(Object* obj)
	{
		brpc::Number* number = cast2noAndCheck(obj, "short", brpc::PRED_SHORT);
		return (short)number->toInt();
	}
	static inline Object* toObject(const short& val)
	{
		return new brpc::Short(val);
	}
};

//类型转换器(Object* <==> int)
template <>
struct Converter<int>
{
	static inline int valueOf(Object* obj)
	{
		brpc::Number* number = cast2noAndCheck(obj, "int", brpc::PRED_INT);
		return number->toInt();
	}
	static inline Object* toObject(const int& val)
	{
		return new brpc::Integer(val);
	}
};

//类型转换器(Object* <==> lint)
template <>
struct Converter<brpc::lint>
{
	static inline brpc::lint valueOf(Object* obj)
	{
		brpc::Number* number = cast2noAndCheck(obj, "long", brpc::PRED_INT64);
		return number->toLong();
	}
	static inline Object* toObject(const brpc::lint& val)
	{
		return new brpc::Long(val);
	}
};

//类型转换器(Object* <==> float)
template <>
struct Converter<float>
{
	static inline float valueOf(Object* obj)
	{
		brpc::Number* number = cast2noAndCheck(obj, "float", brpc::PRED_FLOAT);
		return number->toFloat();
	}
	static inline Object* toObject(const float& val)
	{
		return new brpc::Float(val);
	}
};

//类型转换器(Object* <==> double)
template <>
struct Converter<double>
{
	static inline double valueOf(Object* obj)
	{
		brpc::Number* number = cast2noAndCheck(obj, "double", brpc::PRED_DOUBLE);
		return number->toDouble();
	}
	static inline Object* toObject(const double& val)
	{
		return new brpc::Double(val);
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
		return new brpc::TString(val);
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
		return Converter<T*>::valueOf(obj);
	}

	static inline Object* toObject(SmartPtr<T> val)
	{
		T* ptr = (T*)val;
		return Converter<T*>::toObject(ptr);
	}
};

//...


}// end of namespace bluemei
