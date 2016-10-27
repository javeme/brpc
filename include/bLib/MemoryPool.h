#ifndef MemoryPool_H_H
#define MemoryPool_H_H
#include "Object.h"
#include "CriticalLock.h"
#include <list>
#include <vector>
#include <map>

namespace bluemei{

/////////////////////////////////////////////////////////////////////
// class MemoryPool
class BLUEMEILIB_API MemoryPool : public Object
{
public:
	virtual ~MemoryPool(){};
public:
	virtual void init(unsigned int initSize,unsigned int maxSize)=0;
	virtual void destroy()=0;

	virtual void* get()=0;
	virtual void release(void* obj)=0;

	virtual unsigned int available()=0;
	virtual bool changeMaxSize(unsigned int maxSize)=0;

	virtual void enableThrow(bool needThrow)=0;
};


/////////////////////////////////////////////////////////////////////
// class SimpleMemoryPool
template<class T>
class BLUEMEILIB_TEMPLATE SimpleMemoryPool : public MemoryPool
{
public:
	SimpleMemoryPool(unsigned int initSize=128, unsigned int maxSize=1024);
	virtual ~SimpleMemoryPool();
public:
	virtual void init(unsigned int initSize,unsigned int maxSize);
	virtual void destroy();

	virtual void* get();
	virtual void release(void* obj);

	virtual unsigned int available();
	virtual bool changeMaxSize(unsigned int maxSize);

	virtual void enableThrow(bool needThrow);
protected:
	virtual bool growSize(unsigned int size);
protected:
	std::list<T*> m_listObject;//用于存取
	std::list<T*> m_listOrigObject;//用于最终释放内存
	unsigned int m_nMaxSize;

	bool m_bNeedThrow;

	CriticalLock m_mutexLock;
};


template<class T>
SimpleMemoryPool<T>::SimpleMemoryPool(unsigned int initSize/*=128*/,
	unsigned int maxSize/*=1024*/)
{
	m_bNeedThrow = true;
	init(initSize, maxSize);
}

template<class T>
SimpleMemoryPool<T>::~SimpleMemoryPool()
{
	destroy();
}

template<class T>
void SimpleMemoryPool<T>::init(unsigned int initSize,unsigned int maxSize)
{
	ScopedLock scopedLock(m_mutexLock);

	if(m_listOrigObject.size() > 0)
		return;
	if(maxSize < initSize)
		maxSize = initSize;
	m_nMaxSize = maxSize;
	for(unsigned int i = 0; i < initSize; i++)
	{
		m_listOrigObject.push_back(new T());
	}
	this->m_listObject = m_listOrigObject;
}

template<class T>
void SimpleMemoryPool<T>::destroy()
{
	ScopedLock scopedLock(m_mutexLock);

	for(auto i = m_listOrigObject.begin(); i != m_listOrigObject.end(); ++i)
	{
		delete *i;
	}
	m_listOrigObject.clear();
	m_listObject.clear();
}

template<class T>
bool SimpleMemoryPool<T>::changeMaxSize(unsigned int maxSize)
{
	ScopedLock scopedLock(m_mutexLock);
	/*int usedSize = m_listOrigObject.size()-m_listObject.size();
	if (maxSize <= usedSize)
	{
		return false;
	}*/
	unsigned int currentSize = m_listOrigObject.size();
	if(maxSize<currentSize)
		return false;

	m_nMaxSize = maxSize;
	return true;
}

template<class T>
void SimpleMemoryPool<T>::enableThrow(bool needThrow)
{
	ScopedLock scopedLock(m_mutexLock);
	m_bNeedThrow = needThrow;
}

template<class T>
bool SimpleMemoryPool<T>::growSize(unsigned int size)
{
	ScopedLock scopedLock(m_mutexLock);

	bool success = true;
	int currentSize = m_listOrigObject.size();
	// if m_bNeedThrow is true, the size can't large than m_nMaxSize
	if(size+currentSize > m_nMaxSize && m_bNeedThrow) {
		size = m_nMaxSize - currentSize;
		success = false;
	}

	for(unsigned int i = 0; i < size; i++)
	{
		T* obj = new T();
		m_listOrigObject.push_back(obj);
		m_listObject.push_back(obj);
	}
	return success;
}

template<class T>
unsigned int SimpleMemoryPool<T>::available()
{
	ScopedLock scopedLock(m_mutexLock);

	return m_listObject.size();
}

template<class T>
void* SimpleMemoryPool<T>::get()
{
	ScopedLock scopedLock(m_mutexLock);

	if(available() == 0) {
		// grow current size items
		int currentSize = m_listOrigObject.size();
		growSize((unsigned int)currentSize);
	}

	if(m_listObject.size() > 0) {
		T* obj = m_listObject.front();
		m_listObject.pop_front();
		return obj;
	}
	else {
		if(m_bNeedThrow)
			throw NullPointerException("SimpleMemoryPool out of memory.");

		// assert(0);
		return null;
	}
}

template<class T>
void SimpleMemoryPool<T>::release(void* obj)
{
	ScopedLock scopedLock(m_mutexLock);

	if(obj != null) {
		if(m_listObject.size() > m_nMaxSize) {
			m_listOrigObject.remove((T*)obj);
			delete obj;
		}
		else {
			m_listObject.push_back((T*)obj);
		}
	}
}


/////////////////////////////////////////////////////////////////////
// class GradeMemoryPools
class BLUEMEILIB_API GradeMemoryPools : public Object
{
protected:
	typedef std::map<unsigned int, MemoryPool*> Pools;
	Pools m_pools;

	static const unsigned int GRADE_START = 4;
	static const unsigned int GRADE_MAX = 10;

public:
	GradeMemoryPools(unsigned int bytesPerPool, unsigned int capacityPerPool=0);
	virtual ~GradeMemoryPools();

protected:
	void init(unsigned int sizePerPool, unsigned int capacityPerPool);
	void destroy();

	MemoryPool* suitablePool(unsigned int size);
public:
	virtual void* alloc(unsigned int size);
	virtual void free(void* ptr, unsigned int size);
};


/////////////////////////////////////////////////////////////////////
// class MemoryPoolManager
class BLUEMEILIB_API MemoryPoolManager : public Object
{
public:
	static MemoryPoolManager& instance()
	{
		static MemoryPoolManager manager;
		return manager;
	}

	MemoryPoolManager() : m_gradePools(1024 * 4) {}
public:
	template<class T>
	MemoryPool* getMemoryPool(unsigned int initSize=128,
		unsigned int maxSize=1024)
	{
		ScopedLock scopedLock(m_mutexLock);

		MemoryPool* pool = new SimpleMemoryPool<T>(initSize, maxSize);
		m_listPool.push_back(pool);
		return pool;
	}

	void releaseMemoryPool(MemoryPool* pool)
	{
		ScopedLock scopedLock(m_mutexLock);

		m_listPool.remove(pool);
		delete pool;
	}
public:
	virtual void* alloc(unsigned int size)
	{
		return m_gradePools.alloc(size);
	}

	virtual void free(void* ptr, unsigned int size)
	{
		return m_gradePools.free(ptr, size);
	}
protected:
	//simple memory pools
	std::list<MemoryPool*> m_listPool;
	//grade memory pools
	GradeMemoryPools m_gradePools;

	CriticalLock m_mutexLock;
};

}//end of namespace bluemei
#endif