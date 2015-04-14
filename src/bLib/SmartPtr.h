/************************************************************************
*AUTHOR: Daniel Liu
*FIRST CREATE: Dec.22 2005
*contact: liulele@ee.buaa.edu.cn
*
*This software is free to use. If there's any problem or you have enhanced
*it, please let me know.
*License Type: MIT
*你可以自由使用这部分代码，但请保留这段声明,无论你是否修改了该代码。
************************************************************************/
/************************************************************************
 *NOTE: if this file is compile into a DLL with MSVC, it must be linked 
 *with `Multi-thread DLL runtime library /MD', so it can use the same local
 *heap with the main program. otherwise, GC will fail to get the size of
 *a heap block
 *Your own application that use the DLL must use same character set with 
 *the DLL, e.g. MCBS, otherwise, there will be little memory leak of the 
 *DLL itself, but not the collector.
 ***********************************************************************/

#ifndef __SMARTPTR_H__
#define __SMARTPTR_H__

/*
Characters   
1. reference based and ciruit immunity.
2. determinative deconstructed and no-determinative deconstruct
3. polymorph supported. assign a derived-type point to a base-type point is possible
4. Array supported
5. mutithread safety
6. both primitive type and complex type are supported
7. use just like a raw point
8. new operator is leave not overloaded. you can overload it as you want
9. cross platform, Both Windows and Linux are supported
10. multi-inheritance supported 
11. run-time Array boundary check
//6. use in both single thread or multi thread model

constraints
1. don't use raw point, if you has that point managed by GC
2. don't use point to SmartPtr, but object only. That's to say, SmartPtr<A> p = .. not SmartPtr<A>* p = ..
3. avoid use multi-inheritence, if it is possible
4. avoid overload operator new, if it is possible
5. if you want pass a point to a function that accept raw proint only, such as CDC::SelectObject,
   be caution, you'd better don't use this SmartPtr. or I can add another more function to support
   external reference.
6. when you are define a point, use the most derived type if it is possible.
   such as, class B derived from A. 
   <I>class B:A{}<I>
   use 
   <I>SmartPtr<B> p = new B;<I> 
   but not
   <I>SmartPtr<A> p = new B;<I>
   if it is possible, though this cause no error.

*/
#include "SmartPtrManager.h"
#include "System.h"
#include "TypeManager.h"

#include "MultiValueHashMap.h"


namespace bluemei{

#define NO_USE_EXTENDS 0


template <typename T>
struct DestructorStruct
{
	static void destroy(void* obj) {
		T* p=static_cast<T*>(obj);
		delete p;
	}
};
template <typename T>
struct ArrayDestructorStruct
{
	static void destroy(void* obj) { 
		delete[] static_cast<T*>(obj); 
	}
};


template<typename T> 
class SmartPtr : public LinkNode
{
protected:
#ifdef _DEBUG
	T* targetObj;
#else
	int reserved; //reserved
#endif
	ObjectWrapper *pWrapper;
public:
	SmartPtr<T>& operator=(T* p)
	{
		GlobalMutexLock l;
		ptrTrace("SmartPtr operator=(T* p)\r\n");

		ObjectWrapper *old = pWrapper;
		pWrapper = WrapperManager::getInstance()->getWrapper(p,&DestructorStruct<T>::destroy);
		old->disattach();
#ifdef _DEBUG
		targetObj = (T*)pWrapper->getTarget();
#endif

		return *this;
	}
	SmartPtr<T>& operator=(const SmartPtr<T> &ptr)
	{
		GlobalMutexLock l;
		ptrTrace("SmartPtr operator=(const SmartPtr<T> &ptr)\r\n");
		if(pWrapper == ptr.pWrapper)//assign to self
			return *this;
		pWrapper->disattach();
		pWrapper = ptr.pWrapper;
		pWrapper->attach();
#ifdef _DEBUG
		targetObj = (T*)pWrapper->getTarget();
#endif
		return *this;
	}
	SmartPtr(const SmartPtr<T>& ptr)
	{
		GlobalMutexLock l;
		SmartPtrManager::getInstance()->add(this);
		ptrTrace("SmartPtr con (const SmartPtr<T>& ptr) %p\r\n",this);
		pWrapper = ptr.pWrapper;
		pWrapper->attach();
#ifdef _DEBUG
		targetObj = (T*)pWrapper->getTarget();
#endif
		//type = PTR_UNKNOWN;
	}
	SmartPtr(T* pObj=NULL)
	{
		GlobalMutexLock l;

		TypeManager* manager=TypeManager::instance();
		manager->registerType<T>();

		SmartPtrManager::getInstance()->add(this);
		ptrTrace("SmartPtr con (T* pObj) %p\r\n",this);
		pWrapper = WrapperManager::getInstance()->getWrapper(pObj,&DestructorStruct<T>::destroy);
#ifdef _DEBUG
		targetObj = (T*)pWrapper->getTarget();
#endif
		//type = PTR_UNKNOWN;
	}
public:
	//int operator==(T* p)
	//{
	//	return this->pWrapper->pObj == p;
	//}
	operator const T* () const
	{
		ptrTrace("SmartPtr operator T* \r\n");
		return getTarget();
	}

	operator T* () 
	{
		ptrTrace("SmartPtr operator T* \r\n");
		return getTarget();
	}
protected:
	inline const T* getTarget() const
	{
#if NO_USE_EXTENDS
		return static_cast<T*> (pWrapper->getTarget());
#else
		TypeManager* manager=TypeManager::instance();
		return manager->cast<T>(pWrapper->getTarget());
#endif
	}

	inline T* getTarget() 
	{
#if NO_USE_EXTENDS
		return static_cast<T*> (pWrapper->getTarget());
#else
		TypeManager* manager=TypeManager::instance();
		return manager->cast<T>(pWrapper->getTarget());
#endif
	}

public:
	T* operator->() {return getTarget();}
	const T* operator->() const {return getTarget();}
	~SmartPtr(void)
	{
		GlobalMutexLock l;
		ptrTrace("SmartPtr decon %p\r\n",this);
		SmartPtrManager::getInstance()->remove(this);
		if(System::getInstance().isCollecting())
			return;
		pWrapper->disattach();
	}


	//template<typename P>
	//operator P* () 
	//{
	//	return static_cast<P*>(getTarget());
	//}

	template<typename S>
	SmartPtr<S> staticCast(){ return static_cast<S*>(getTarget()); }
	template<typename S>
	const SmartPtr<S> staticCast() const { return static_cast<S*>(getTarget()); }
	
	template<typename S>
	SmartPtr<S> dynamicCast(){ return dynamic_cast<S*>(getTarget()); }
	template<typename S>
	const SmartPtr<S> dynamicCast() const { return dynamic_cast<S*>(getTarget()); }

	friend class System;

	/*template<typename S, typename V>
	friend SmartPtr<S> ptr_static_cast(SmartPtr<V> ptr);

	template<typename S, typename V>
	friend SmartPtr<S> ptr_dynamic_cast(SmartPtr<V> ptr);*/
};

template<typename T> 
class ArrayPtr : public SmartPtr<T>
{
	//up bound of array
	int upBound;
public:
	ArrayPtr<T>& operator=(T* p)
	{
		SmartPtr<T>::operator=(p);
		SmartPtr<T>::pWrapper->setFinalizer(&ArrayDestructorStruct<T>::destroy);
		upBound = SmartPtr<T>::pWrapper->getSize()/sizeof(T);
		return *this;
	}
	ArrayPtr<T>& operator=(const ArrayPtr<T> &ptr)
	{
		SmartPtr<T>::operator=(ptr);
		upBound = ptr->upBound;
		return *this;
	}
	ArrayPtr(const ArrayPtr<T>& ptr):SmartPtr<T>(ptr)
	{
		upBound = ptr.upBound;
	}
	ArrayPtr(T pObj[]=NULL):SmartPtr<T>(pObj)
	{
		SmartPtr<T>::pWrapper->setFinalizer(&ArrayDestructorStruct<T>::destroy);
		///determine size of an array block
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4267)
#endif
		upBound = SmartPtr<T>::pWrapper->getSize()/sizeof(T);
#ifdef _MSC_VER
#pragma warning (pop)
#endif
	}
	//operator T* ()
	//{
	//	return getTarget();
	//}

	//inline T* getTarget()
	//{
	//	return static_cast<T*> (pWrapper->getTarget());
	//}
	T* operator->(){return SmartPtr<T>::getTarget();}
	T& operator[](int index)
	{
		return SmartPtr<T>::getTarget()[index];
	}
public:
	~ArrayPtr(void)
	{
		ptrTrace("ArrayPtr decon %p\r\n",this);
	}
	friend class System;
};

template<typename S, typename T>
SmartPtr<S> ptr_static_cast(SmartPtr<T> ptr) { return ptr.staticCast<S>(); }

template<typename S, typename T>
SmartPtr<S> ptr_dynamic_cast(SmartPtr<T> ptr) { return ptr.dynamicCast<S>(); }


template<> Value2String<SmartPtr<Object>>::operator String()const{
	if(value==nullptr)
		return "<null>";
	return value->toString();
}template class BLUEMEILIB_API Value2String<SmartPtr<Object>>;


template class BLUEMEILIB_API HashMap<String,SmartPtr<Object>>;
template class BLUEMEILIB_API MultiValueHashMap<String,SmartPtr<Object>>;

}//end of namespace bluemei
#endif //__SMARTPTR_H__
