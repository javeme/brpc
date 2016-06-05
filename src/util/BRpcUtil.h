#pragma once
#include "stdafx.h"
#include "blib.h"
#include "ObjectRef.h"
#include "TypeConverter.h"
#include "RpcException.h"


namespace brpc{
	
/*
* 字符及其编码工具类
* @author Javeme
* @create 2014/7/5
*/
class BRpcUtil : public bluemei::CodeUtil
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


/////////////////////////////////////////////////////////////
// tow strings are the same value
static bool streq(cstring str1, cstring str2)
{
	return strcmp(str1, str2) == 0;
}


/////////////////////////////////////////////////////////////
// Object <==> ObjectMap相互转换
struct MapObjectConverter
{
	static Object* object2map(Object* obj);
	static Object* map2object(Object* map, const Class* cls);
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
		const Class* cls = Cls::thisClass();
		return dynamic_cast<Type>(MapObjectConverter::map2object(map, cls));
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
* @author Javeme
* @create 2014/7/4
*/
template <typename Type>
inline Type valueOf(Object* obj)
{
	checkNullPtr(obj);

	try{
		return brpc::Converter<Type>::valueOf(obj);
	}catch(BadCastException&){
		//Type is a sub-Object pointer?
		const bool isSubObjectPtr = bluemei::is_convertible<Type, Object*>::value;
		bool needMap2Object = false;
		if (isSubObjectPtr) {
			needMap2Object = true;
			try{
				//parse a map to a object
				return MapObjectHelper<Type, isSubObjectPtr>::map2object(obj);
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
* @author Javeme
* @create 2014/7/4
*/
template <typename Type>
inline Object* toObject(const Type& val)
{
	return brpc::Converter<Type>::toObject(val);
}

template <typename Type>
inline Object* objectToMap(Type obj)
{
	//Type is a sub-Object
	const bool isSubObject = is_convertible<Type, Object*>::value;
	return MapObjectHelper<Type, isSubObject>::object2map(obj);
}

template <typename Type>
inline Type mapToObject(Object* map)
{
	//Type is a sub-Object
	const bool isSubObject = is_convertible<Type, Object*>::value;
	return MapObjectHelper<Type, isSubObject>::map2object(map);
}


/////////////////////////////////////////////////////////////
// TypeObjectHelper for TypeObject
template<typename Type, typename bool>
struct TypeObjectHelper;

template<typename Type>
struct TypeObjectHelper<Type, true>
{
	static bool deletePtr(Type& ptr)
	{
		delete ptr;
		ptr = null;
		return true;
	}
	static Type setNull(Type& ptr)
	{
		Type val = ptr;
		ptr = null;
		return val;
	}
};

template<typename Type>
struct TypeObjectHelper<SmartPtr<Type>, true>
{
	static bool deletePtr(SmartPtr<Type>& ptr)
	{
		ptr = null;
		return false;
	}
	static SmartPtr<Type> setNull(SmartPtr<Type>& ptr)
	{
		SmartPtr<Type> val = ptr;
		ptr = null;
		return val;
	}
};

template<typename Type>
struct TypeObjectHelper<Type, false>
{
	static bool deletePtr(Type& ptr)
	{
		return false;
	}
	static Type setNull(Type& ptr)
	{
		return ptr;
	}
};

// for auto release type ptr
template <typename Type>
class TypeObject : public Object
{
public:
	TypeObject(const Type& val=Type(), bool dontDelete=false)
		: m_val(val), m_dontDelete(dontDelete) {
	}

	~TypeObject() {
		if(!this->m_dontDelete)
			deletePtr();
	}

	TypeObject(TypeObject&& other) {
		*this = std::move(other);
	}

	TypeObject& operator=(TypeObject&& other) { 
		this->m_val = other.detach();
		this->m_dontDelete = other.m_dontDelete;
		return *this; 
	}

public:
	operator Type () const {
		return m_val;
	}

	operator Type& () {
		return m_val;
	}

	Type detach() {
		const bool IS_PTR = bluemei::is_convertible<Type, void*>::value;
		return TypeObjectHelper<Type, IS_PTR>::setNull(m_val);
	}

protected:
	void deletePtr() {
		const bool IS_PTR = bluemei::is_convertible<Type, void*>::value;
		(void)TypeObjectHelper<Type, IS_PTR>::deletePtr(m_val);
	}

private:
	TypeObject(const TypeObject& other);
	TypeObject& operator = (const TypeObject& other) const;

protected:
	Type m_val;
	bool m_dontDelete;
};


// arg object to type object
template <typename Type>
inline TypeObject<Type> methodArg(Object* obj)
{
	checkNullPtr(obj);
	ObjectRef* ref = dynamic_cast<ObjectRef*>(obj);
	if (ref)
	{
		Object* originalObj = ref->getObject();
		//this should return originalObj;
		return TypeObject<Type>(valueOf<Type>(originalObj), true);
	}
	else
	{
		return valueOf<Type>(obj);
	}
}


/////////////////////////////////////////////////////////////
// Match args
enum MatchLevel{ LEVEL_NOT_MATCHED, LEVEL_CAN_CONVERT, LEVEL_MATCHED };
template <typename Type>
inline MatchLevel matchLevel(Object* obj)
{
	try{
		TypeObject<Type> val = methodArg<Type>(obj);
		
		Number* number = dynamic_cast<Number*>(obj);
		if (number != null)
		{
			SmartPtr<Object> objBack = toObject<Type>(val);
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


/////////////////////////////////////////////////////////////
// string_caster
template <typename Type, typename bool>
struct string_caster;

template <typename Type>
struct string_caster<Type, true>
{
	static String toString(const Type& val){
		return String::format("'%s'", String(val).c_str());
	}
};

template <typename Type>
struct string_caster<Type, false>
{
	static String toString(const Type& val){
		return Value2String<Type>(val);;
	}
};

}//end of namespace brpc
