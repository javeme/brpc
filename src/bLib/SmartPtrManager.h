#ifndef SmartPtrManager_H
#define SmartPtrManager_H
#include "bluemeiLib.h"

#include <malloc.h>
#include <set>

#ifdef WIN32
//#include <windows.h> //do not include windows.h, this may cause starnge compile time error
#ifndef _DLL//MTd调用_msize()时出现错误: HEAP[]: Invalid address specified to RtlValidateHeap( 02160000, 05C6C8C8 )
#error To use the util library, please compile the project with "Multithread DLL (/MD)" or "Multithread Debug DLL (/MDd)" 
#endif
#endif

namespace bluemei{

class System;


typedef void (*DESTORY_PROC)(void*);
//extern bool collecting ;

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

#ifdef WIN32
class  BLUEMEILIB_API GlobalMutexLock
{
public:

	GlobalMutexLock();
	~GlobalMutexLock();
};
#else

class BLUEMEILIB_API GlobalMutexLock
{
	static pthread_mutex_t mutex;
public:
	GlobalMutexLock()
	{
		static bool inited=false;
		if(!inited)
		{
			pthread_mutexattr_t attr;
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init (&mutex,&attr);
			inited=true;
		}
		pthread_mutex_lock (&mutex);
	}
	~GlobalMutexLock()
	{
		pthread_mutex_unlock(&mutex);
		
	}
};
#endif

class BLUEMEILIB_API LinkNode
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
class BLUEMEILIB_API ObjectWrapper 
{
	int count;	//reference count
	void *pTarget;	//the real object allocated by usrer;
	DESTORY_PROC destory; //a routine to call delete method
	bool isInUse;
	size_t size;	//size of pObj. may not equal sizeof(T)
private:
	~ObjectWrapper();
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
class BLUEMEILIB_API WrapperPointer
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
class  BLUEMEILIB_API WrapperManager 
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

	ObjectWrapper* nullWrapper;
/**
	return an element of ObjectWrapper list, where pTarget should 
	be insert after.
	return pos, where pos->pTarget <= pTarget < (pos+1)->pTarget
	<B> NOTE: not used now. use STL set to store wrapper objects.
	*/
	//ObjectWrapper* positionToInsert(void *pTarget);
	WrapperManager();
	~WrapperManager();
public:
	/**
	size of extra bytes more than requested allocated by new.
	*/
	static int extraSize;
	/**
	size reserved at head of the memory block
	*/
	static int arrayHeadSize;
	void remove(ObjectWrapper* pWrapper);
	void collect(ObjectWrapper* pWrapper);
	/**
	if target has already exist in the list, just return the old Wrapper.
	this allow to cast a derived-type point to a base-type point. 
	this make multi-inheritance enabled
	else create a new Wrapper
	If a new Wrapper is create and returned, the new Wrapper has reference count 1
	If an exists Wrapper is returned, the reference count is add 1 before return
	*/
	//template <typename T>
	ObjectWrapper* getWrapper(void* pTarget, DESTORY_PROC destory)
	{
		GlobalMutexLock l;
		if(pTarget == NULL)
			return nullWrapper;

		// a non zero memSize make the ObjectWrapper use this size instead of calculate size itself
		// which save the computer resource
		ObjectWrapper tempWrapper(pTarget, NULL, 1);

		
		WrapperSet::const_iterator i = wrappers.find(&tempWrapper);
		if(i == wrappers.end())
		{
			ObjectWrapper* pW = new ObjectWrapper(pTarget, destory);
			wrappers.insert(pW);
			return pW;
		}
		else
		{
			(*i).p->attach();
			return (*i).p;
		}
	}
	bool isEmbeddedPtr(void *pSmartPtr);
	void destroy();
	static WrapperManager* getInstance();
	friend class System;
};

class BLUEMEILIB_API SmartPtrManager
{
	LinkNode *pHead;
	LinkNode *pTail;
	SmartPtrManager();
	~SmartPtrManager();

	LinkNode *pUserPtrHead;
	LinkNode *pUserPtrTail;

	LinkNode *pEmbeddedPtrHead;
	LinkNode *pEmbeddedPtrTail;

public:
	static SmartPtrManager* getInstance();
	void add(LinkNode* ptr);
	void remove(LinkNode* ptr);
	void moveToUserPtr(LinkNode* ptr);
	void moveToEmbeddedPtr(LinkNode* ptr);
	void destroy();
	friend class System;
};


}//end of namespace bluemei
#endif