#pragma once
#include "stdafx.h"
#include "blib/CodeUtil.h"
#include "bLib/SmartPtr.h"
#include "bLib/BeanContainer.h"
#include "src/type/ObjectRef.h"
#include "src/type/TypeConverter.h"
#include "src/util/RpcException.h"

namespace brpc{

using namespace blib;
using blib::byte;

/**
 * 字符及其编码工具类
 * @author Javeme
 * @create 2014/7/5
 */
class BRpcUtil : public blib::CodeUtil
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

template<typename Type, bool convertible>
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
		typedef typename Ptr2Type<Type>::Type Cls;
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


/**
 * Object对象转换为指定类型(无法转换时抛出异常)
 * @author Javeme
 * @create 2014/7/4
 */
template <typename Type> class RealTypeWrapper;
#define RealTypeOf(Type) typename RealTypeWrapper<Type>::RealType

template <typename Type>
inline RealTypeOf(Type) valueOf(Object* obj)
{
	checkNullPtr(obj);

	try{
		// NOTE: if the `Type` itself is a reference(like 'const &' or '&'),
		// the value memory would be released here when returning a reference,
		// so let's return the original type `brpc::Converter<Type>::RealType`
		return brpc::Converter<Type>::valueOf(obj);
	}catch(BadCastException&){
		//is `Type` a sub-Object pointer?
		const bool IS_SUB_OBJ = blib::is_convertible<
			RealTypeOf(Type), Object*>::value;
		bool needMap2Object = false;
		if (IS_SUB_OBJ) {
			needMap2Object = true;
			try{
				//parse a map to a object(a pointer of subclass of Object)
				return MapObjectHelper<Type, IS_SUB_OBJ>::map2object(obj);
			}catch(NotMapException&){
				needMap2Object = false;
			}
		}
		//needMap2Object must be false
		assert(!needMap2Object);
		//rethrow exception if there is no need to conv map to object
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
	const bool IS_SUB_OBJ = blib::is_convertible<Type, Object*>::value;
	return MapObjectHelper<Type, IS_SUB_OBJ>::object2map(obj);
}

template <typename Type>
inline Type mapToObject(Object* map)
{
	//Type is a sub-Object
	const bool IS_SUB_OBJ = blib::is_convertible<Type, Object*>::value;
	return MapObjectHelper<Type, IS_SUB_OBJ>::map2object(map);
}


/////////////////////////////////////////////////////////////
// TypeWrapperHelper for TypeWrapper
template<typename Type, bool convertible>
struct TypeWrapperHelper;

template<typename Type>
struct TypeWrapperHelper<Type, true>
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
struct TypeWrapperHelper<SmartPtr<Type>, true>
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
struct TypeWrapperHelper<Type, false>
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

// class TypeWrapper for auto release type if it's a ptr
template <typename Type>
class TypeWrapper : public Object
{
public:
	TypeWrapper(const Type& val=Type(), bool dontDelete=false)
		: m_val(val), m_dontDelete(dontDelete) {
	}

	~TypeWrapper() {
		if(!this->m_dontDelete)
			deletePtr();
	}

	TypeWrapper(TypeWrapper&& other) {
		*this = std::move(other);
	}

	TypeWrapper& operator=(TypeWrapper&& other) {
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
		const bool IS_PTR = blib::is_convertible<Type, void*>::value;
		return TypeWrapperHelper<Type, IS_PTR>::setNull(m_val);
	}

protected:
	void deletePtr() {
		const bool IS_PTR = blib::is_convertible<Type, void*>::value;
		(void)TypeWrapperHelper<Type, IS_PTR>::deletePtr(m_val);
	}

private:
	TypeWrapper(const TypeWrapper& other);
	TypeWrapper& operator = (const TypeWrapper& other) const;

protected:
	Type m_val;
	bool m_dontDelete;
};


template <typename Type>
class RealTypeWrapper : public TypeWrapper<Type>
{
public:
	typedef Type RealType;

	RealTypeWrapper(const Type& val=Type(), bool dontDelete=false)
		: TypeWrapper<Type>(val, dontDelete) {
	}
};

// when the `Type` itself is a reference, let's remove the `&`
template <typename Type>
class RealTypeWrapper<Type&> : public TypeWrapper<Type>
{
public:
	typedef Type RealType;

	RealTypeWrapper(const Type& val=Type(), bool dontDelete=false)
		: TypeWrapper<Type>(val, dontDelete) {
	}
};

template <typename Type>
class RealTypeWrapper<const Type&> : public TypeWrapper<Type>
{
public:
	typedef Type RealType;

	RealTypeWrapper(const Type& val=Type(), bool dontDelete=false)
		: TypeWrapper<Type>(val, dontDelete) {
	}
};


// convert arg object to a value of Type
template <typename Type>
inline RealTypeWrapper<Type> methodArg(Object* obj)
{
	checkNullPtr(obj);
	ObjectRef* ref = dynamic_cast<ObjectRef*>(obj);
	if (ref)
	{
		Object* originalObj = ref->getObject();
		// this should return the `originalObj` which could not be deleted
		const bool IS_OBJ_PTR = blib::is_convertible<
			RealTypeOf(Type), Object*>::value;
		return RealTypeWrapper<Type>(
			blib::dynamic_caster<Type, IS_OBJ_PTR>::toType(originalObj),
			true);
	}
	else
	{
		// even if the `Type` is a reference, valueOf() will return orig type
		return RealTypeWrapper<Type>(valueOf<Type>(obj));
	}
}


/////////////////////////////////////////////////////////////
// match args
enum MatchLevel{ LEVEL_NOT_MATCHED, LEVEL_CAN_CONVERT, LEVEL_MATCHED };

// does an object match the Type
template <typename Type>
inline MatchLevel matchLevel(Object* obj)
{
	try{
		// whether `obj` can be cast to a value of `Type`
		RealTypeWrapper<Type> val = methodArg<Type>(obj);

		// if expected type is number, the `obj` may be not the same type
		// like expected type is int but `obj` is a Double, and we should
		// return LEVEL_CAN_CONVERT, return LEVEL_MATCHED if the same type
		Number* number = dynamic_cast<Number*>(obj);
		if (number != null)
		{
			ScopePointer<Object> objBack = toObject<Type>(val);
			Number* numberBack = dynamic_cast<Number*>((Object*)objBack);
			if (numberBack != null && numberBack->isSameType(*number)){
				return LEVEL_MATCHED;
			}
			return LEVEL_CAN_CONVERT;
		}
		// expected type is not number
		else
			return LEVEL_MATCHED;
	}catch (Exception&)
	{
		return LEVEL_NOT_MATCHED;
	}
}


/////////////////////////////////////////////////////////////
// string_caster
template <typename Type, bool>
struct string_caster;

template <typename Type>
struct string_caster<Type, true>
{
	static String toString(const Type& val){
		String str = val;
		return String::format("'%s'", str.c_str());
	}
};

template <typename Type>
struct string_caster<Type, false>
{
	static String toString(const Type& val){
		return Value2String<Type>(val);
	}
};

}//end of namespace brpc
