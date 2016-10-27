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


namespace bluemei{

//////////////////////////////////////////////////////////////////////
//class SmartPtr
template<typename T>
class SmartPtr : public LinkNode
{
public:
	SmartPtr(T* pObj=nullptr)
	{
		GlobalMutexLock l;

		SmartPtrManager::getInstance()->add(this);
		ptrTrace("SmartPtr con (T* pObj) %p\r\n",this);
		getWrapper(pObj);

		m_targetObj = pObj;
		m_reserved = 0;
	}

	virtual ~SmartPtr(void)
	{
		GlobalMutexLock l;
		ptrTrace("SmartPtr decon %p\r\n",this);
		SmartPtrManager::getInstance()->remove(this);
		//maybe not exists this scenarios due to GlobalMutexLock
		if(System::instance().isCollecting())
			return;
		m_wrapper->disattach();
	}

	SmartPtr(const SmartPtr<T>& ptr)
	{
		GlobalMutexLock l;
		SmartPtrManager::getInstance()->add(this);
		ptrTrace("SmartPtr con (const SmartPtr<T>& ptr) %p\r\n",this);
		m_wrapper = ptr.m_wrapper;
		m_wrapper->attach();

		m_targetObj = ptr.m_targetObj;
		m_reserved = ptr.m_reserved;
	}

	SmartPtr<T>& operator=(const SmartPtr<T> &ptr)
	{
		GlobalMutexLock l;
		ptrTrace("SmartPtr operator=(const SmartPtr<T> &ptr)\r\n");
		if(m_wrapper == ptr.m_wrapper)//assign to self
			return *this;
		m_wrapper->disattach();
		m_wrapper = ptr.m_wrapper;
		m_wrapper->attach();

		m_targetObj = ptr.m_targetObj;
		m_reserved = ptr.m_reserved;

		return *this;
	}

	SmartPtr<T>& operator=(T* pObj)
	{
		GlobalMutexLock l;
		ptrTrace("SmartPtr operator=(T* p)\r\n");

		ObjectWrapper *old = m_wrapper;
		getWrapper(pObj);
		old->disattach();

		m_targetObj = pObj;

		return *this;
	}
public:
	/*int operator==(T* p)
	{
		return this->pWrapper->pObj == p;
	}*/
	int operator==(T* p) const {
		return this->m_targetObj == p;
	}

	operator T* () const {
		return getTarget();
	}

	T* operator->() const {
		return getTarget();
	}

public:
	template<typename S>
	S* asType() const { return static_cast<S*>(getTarget()); }

	template<typename S>
	SmartPtr<S> staticCast(){ return static_cast<S*>(getTarget()); }

	template<typename S>
	const SmartPtr<S> staticCast() const { return static_cast<S*>(getTarget()); }

	template<typename S>
	SmartPtr<S> dynamicCast(){ return dynamic_cast<S*>(getTarget()); }

	template<typename S>
	const SmartPtr<S> dynamicCast() const { return dynamic_cast<S*>(getTarget()); }

	/*template<typename S, typename V>
	friend SmartPtr<S> ptr_static_cast(SmartPtr<V> ptr);

	template<typename S, typename V>
	friend SmartPtr<S> ptr_dynamic_cast(SmartPtr<V> ptr);*/
protected:
	inline T* getTarget() const {
		return m_targetObj;
	}

	inline void* getTargetAddr() {
		return m_wrapper->getTarget();
	}

	ObjectWrapper* getWrapper(T* pObj) {
		this->m_wrapper = WrapperManager::getInstance()->getWrapper(pObj);
		return this->m_wrapper;
	}
protected:
	T* m_targetObj;
	int m_reserved; //reserved

	ObjectWrapper *m_wrapper;
	friend class System;
};


//////////////////////////////////////////////////////////////////////
//class ArrayPtr
template<typename T>
class ArrayPtr : public SmartPtr<T>
{
public:
	ArrayPtr<T>& operator=(T* p)
	{
		SmartPtr<T>::operator=(p);
		SmartPtr<T>::m_wrapper->setFinalizer(&ArrayDestructorStruct<T>::destroy);
		m_upBound = SmartPtr<T>::m_wrapper->getSize()/sizeof(T);
		return *this;
	}
	ArrayPtr<T>& operator=(const ArrayPtr<T> &ptr)
	{
		SmartPtr<T>::operator=(ptr);
		m_upBound = ptr->upBound;
		return *this;
	}
	ArrayPtr(const ArrayPtr<T>& ptr):SmartPtr<T>(ptr)
	{
		m_upBound = ptr.upBound;
	}
	ArrayPtr(T pObj[]=NULL):SmartPtr<T>(pObj)
	{
		SmartPtr<T>::m_wrapper->setFinalizer(&ArrayDestructorStruct<T>::destroy);
		///determine size of an array block
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4267)
#endif
		m_upBound = SmartPtr<T>::m_wrapper->getSize()/sizeof(T);
#ifdef _MSC_VER
#pragma warning (pop)
#endif
	}

	virtual ~ArrayPtr(void)
	{
		ptrTrace("ArrayPtr decon %p\r\n",this);
	}

public:
	T* operator->(){return SmartPtr<T>::getTarget();}
	T& operator[](int index)
	{
		return SmartPtr<T>::getTarget()[index];
	}

	friend class System;
protected:
	//up bound of array
	int m_upBound;
};

template<typename S, typename T>
SmartPtr<S> ptr_static_cast(SmartPtr<T> ptr) { return ptr.staticCast<S>(); }

template<typename S, typename T>
SmartPtr<S> ptr_dynamic_cast(SmartPtr<T> ptr) { return ptr.dynamicCast<S>(); }


template<> Value2String<SmartPtr<Object>>::operator String() const{
	if(value==nullptr)
		return "<null>";
	return value->toString();
}template class BLUEMEILIB_API Value2String<SmartPtr<Object>>;


template class BLUEMEILIB_API HashMap<String,SmartPtr<Object>>;
template class BLUEMEILIB_API MultiValueHashMap<String,SmartPtr<Object>>;

}//end of namespace bluemei
#endif //__SMARTPTR_H__
