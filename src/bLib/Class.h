#ifndef Class_H_H
#define Class_H_H
#include "bluemeiLib.h"
#include "ClassField.h"

namespace bluemei{
	
class Object;
class ClassField;
class RuntimeException;
template <typename T> class ArrayList;
typedef Map<std::string, FieldInfo*> FieldMap;

//创建对象函数声明
typedef Object* CreateFun(void);

//Object Metadata
class BLUEMEILIB_API Class
{
public:
	Class();
	Class(cstring name,CreateFun* pFun,const Class* superClass=nullptr);
	virtual ~Class();
public:
	Object* createObject()const;
	cstring getName()const;
	bool operator==(const Class& other)const;
	const Class* superClass() const { 
		return m_superClassRef; 
	}

	Object* getInitPara() const { 
		return m_pInitPara; 
	}
	void setInitPara(Object* val) { 
		m_pInitPara = val;
	}
	bool isMyObject(const Object* pObj)const;//是否就是本类实例
	bool isMyInstance(const Object* pObj)const;//是否属于本类实例
	
	bool putField(const FieldInfo& fld);
	bool removeField(cstring fldName);

	ArrayList<const FieldInfo*> allFields() const;
	const FieldInfo* getField(cstring fldName) const;
	bool hasField(cstring fldName) const;
public:
	static const Class* undefined();
	static void registerClass(Class* cls);
	static Class* registerClass(cstring name,CreateFun* pFun,const Class* superClass=nullptr);
	static Class* registerClassIfNotExist(cstring name,CreateFun* pFun,const Class* superClass=nullptr);
	static void throwRuntimeException(cstring msg) throw(RuntimeException);
protected:
	std::string m_name;
	CreateFun* m_pCreateFun;
	const Class* m_superClassRef;
	Object* m_pInitPara;
	FieldMap m_fields;
};

//获取反射类
#define CLASS(className)\
	Class(_T2STR(className),className::createObject)
#define NEW_CLASS(className)\
	new Class(_T2STR(className),className::createObject)


//可动态创建类声明宏
#define DECLARE_DCLASS(className) \
	typedef className Self;																	   \
	static Class* thisClass(){																   \
		static Class* cls = Class::registerClass(_T2STR(className),							   \
												 Self::createObject,__super::thisClass());	   \
		return cls;																			   \
	}																						   \
	static Object* createObject(){ return new Self();}										   \
	virtual Object* clone()const{ return new Self(*this);}									   \
	virtual const Class* getThisClass() const { 											   \
		const Class* cls=thisClass();														   \
		if(!cls->isMyObject(this))															   \
		{																					   \
			Class::throwRuntimeException("The object is not a instance of class '"			   \
				_T2STR(className)"'(Please implement DECLARE_DCLASS)");						   \
			return Class::undefined();														   \
		}																					   \
		else																				   \
			return cls;																		   \
	}							   															   \
	template <typename T>																	   \
	void setAttributeT(cstring name, const T& val) {										   \
		const FieldInfo* fldInfo = getThisClass()->getField(name);							   \
		typedef const FieldType<T(Self::*)>* TheFieldType;									   \
		TheFieldType fldType= dynamic_cast<TheFieldType>(fldInfo);							   \
		if(fldType)																			   \
			return fldType->set(*this, val);												   \
		else {																				   \
			String err = String::format("type<%s> not matched, should be type<%s>", 		   \
				typeid(T).name(), fldInfo->typeInfo().name());								   \
			throwpe(TypeException(err));													   \
		}																					   \
	}																						   \
	template <typename T>																	   \
	T getAttributeT(cstring name) {															   \
		const FieldInfo* fldInfo = getThisClass()->getField(name);							   \
		typedef const FieldType<T(Self::*)>* TheFieldType;									   \
		TheFieldType fldType= dynamic_cast<TheFieldType>(fldInfo);							   \
		if(fldType)																			   \
			return fldType->get(*this);														   \
		else {																				   \
			String err = String::format("type<%s> not matched, should be type<%s>", 		   \
				typeid(T).name(), fldInfo->typeInfo().name());								   \
			throwpe(TypeException(err));													   \
		}																					   \
	}																						   \
//end of DECLARE_DCLASS

}//end of namespace bluemei

#endif