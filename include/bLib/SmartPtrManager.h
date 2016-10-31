#ifndef SmartPtrManager_H
#define SmartPtrManager_H
#include "bluemeiLib.h"
#include "Object.h"
#include "MutexLock.h"
#include "TypeManager.h"

#include <malloc.h>
#include <set>

#ifdef WIN32
//#include <windows.h> //do not include windows.h, this may cause starnge compile time error
#ifndef _DLL//MTd调用_msize()时出现错误: HEAP[]: Invalid address specified to RtlValidateHeap( 02160000, 05C6C8C8 )
#error To use the util library, please compile the project with "Multithread DLL (/MD)" or "Multithread Debug DLL (/MDd)"
#endif
#endif

namespace blib{

class System;


typedef void (*DESTORY_PROC)(void*);

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

#ifdef _DEBUG
#define ptrTrace System::debugInfo
#else
#define ptrTrace(...)
#endif

//three types of a SmartPtr that can be. it be UNKNOW when it is created.
//if a SmartPtr object is iner another object that managed by GC, the type of SmartPtr is MANAGED
//else, a SmartPtr is managed by user self, type is USER
#define PTR_UNKNOWN 0
#define PTR_USER	1
#define PTR_MANAGED	2


class BLUEMEILIB_API GlobalMutexLock : public Object
{
public:
	GlobalMutexLock();
	virtual ~GlobalMutexLock();
private:
	static MutexLock s_globalMutex;
};


class BLUEMEILIB_API LinkNode : public Object
{
	LinkNode *pPrev;
	LinkNode *pNext;
public:
	LinkNode()
	{
		pNext = NULL;
		pPrev = NULL;
	}
	friend class SmartPtrManager;
	friend class System;
};

class WrapperPointer;
class BLUEMEILIB_API ObjectWrapper : public Object
{
private:
	int count;	//reference count
	void *pTarget;	//the real object allocated by usrer;
	DESTORY_PROC destory; //a routine to call delete method
	bool isInUse;
	size_t size;	//size of pObj. may not equal sizeof(T)
private:
	virtual ~ObjectWrapper();
	ObjectWrapper(void * p,DESTORY_PROC destory,size_t memSize=0);
public:
	void attach();
	void disattach();
	void* getTarget();
	size_t getSize(){return size;}
	void setFinalizer(DESTORY_PROC finalize);
	bool contain(void* point);
	friend class WrapperManager;
	friend class System;
	friend WrapperPointer;
};


class ObjectWrapper;
/**
 * struct WrapperPointer is a simple struct contains a ObjectWrapper pointer. We want put a ObjectWrapper* into a
 * STL set, but the compiler doesn't allow us to define a operator < for a pointer. We must encapsulate the pointer
 * as a struct, then overload the operator < for the struct
 */
class BLUEMEILIB_API WrapperPointer : public Object
{
public:
	ObjectWrapper* p;
	WrapperPointer(ObjectWrapper* p) { this->p = p; }
	WrapperPointer() {}

	bool operator<(const WrapperPointer& other) const
	{
		return ((char*)(p->pTarget) + p->size) < other.p->pTarget;
	}
	bool operator==(const WrapperPointer& other) const
	{
		return p==other.p;
	}
};


/**
manage all the ObjectWrapper and ArrayWrapper.
*/
class BLUEMEILIB_API WrapperManager : public Object
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning( disable : 4251)
#endif
	typedef std::set<WrapperPointer> WrapperSet;
	WrapperSet wrappers;
#ifdef _MSC_VER
#pragma warning(pop)
#endif
/**
	return an element of ObjectWrapper list, where pTarget should
	be insert after.
	return pos, where pos->pTarget <= pTarget < (pos+1)->pTarget
	<B> NOTE: not used now. use STL set to store wrapper objects.
	*/
	//ObjectWrapper* positionToInsert(void *pTarget);
	WrapperManager();
	virtual ~WrapperManager();
	friend class System;
public:
	static WrapperManager* getInstance();
public:
	void remove(ObjectWrapper* pWrapper);
	void collect(ObjectWrapper* pWrapper);

	bool isEmbeddedPtr(void *pSmartPtr);
	void destroy();
public:

	/**
	if target has already exist in the list, just return the old Wrapper.
	this allow to cast a derived-type point to a base-type point.
	this make multi-inheritance enabled
	else create a new Wrapper
	If a new Wrapper is create and returned, the new Wrapper has reference count 1
	If an exists Wrapper is returned, the reference count is add 1 before return
	*/
	template <typename T>
	ObjectWrapper* getWrapper(T* pTarget, bool canUseSuperSize=true)
	{
		void* ptr = Conver::ptrOfType(pTarget);
		size_t memSize = 0;
		//memSize=0 meas get the size of pTarget by os-api.
		//maybe T is not the class(is super class) of pTarget;
		//set memSize = sizeof(T) if canUseSuperSize has been setted.
		if (canUseSuperSize){
			memSize = sizeof(T);
		}
		//if canUseSuperSize has not been setted, and at the first time,
		//make sure that pTarget is itself (rather than super-class obj).
		else if(!existPtr(ptr))
			checkPtrAddr<T>(ptr);

		if(pTarget == NULL)
			return nullWrapper;
		else
			return attachWrapper(ptr, &DestructorStruct<T>::destroy, memSize);
	}

	//check ptr invalid
	template <typename T>
	void checkPtrAddr(void* ptr)
	{
		/* 指针ptr必须是其本身的地址,不能是其父类地址(偏移地址).
		* 如: class C : public A, public B {}
		* 非法: SmartPtr<B> b = new C();
		* 合法: SmartPtr<B> b = SmartPtr<C>(new C());
		*/
		TypeManager* manager=TypeManager::instance();
		(void)manager->registerType<T>();
		if(!manager->objectIsItselfAddr(ptr))
			throwpe(Exception("SmartPtr pointer must be itself"));
	}
private:
	ObjectWrapper* attachWrapper(void* pTarget, DESTORY_PROC destory, size_t memSize=0);
	bool existPtr(void* pTarget) const;
private:
	static ObjectWrapper* nullWrapper;

public:
	/**
	size of extra bytes more than requested allocated by new.
	*/
	static int extraSize;
	/**
	size reserved at head of the memory block
	*/
	static int arrayHeadSize;
};

class BLUEMEILIB_API SmartPtrManager : public Object
{
public:
	static SmartPtrManager* getInstance();

public:
	void add(LinkNode* ptr);
	void remove(LinkNode* ptr);
	void moveToUserPtr(LinkNode* ptr);
	void moveToEmbeddedPtr(LinkNode* ptr);
	void destroy();
protected:
	SmartPtrManager();
	virtual ~SmartPtrManager();
protected:
	LinkNode *pHead;
	LinkNode *pTail;

	LinkNode *pUserPtrHead;
	LinkNode *pUserPtrTail;

	LinkNode *pEmbeddedPtrHead;
	LinkNode *pEmbeddedPtrTail;

	friend class System;
};


}//end of namespace blib
#endif