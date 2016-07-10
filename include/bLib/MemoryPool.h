#pragma once
#include "Object.h"
#include "LinkedList.cpp"
#include "CriticalLock.h"

namespace bluemei{

class BLUEMEILIB_API MemoryPool : public Object
{
public:
	virtual ~MemoryPool(){};
public:
	virtual void init(unsigned int itemSize,unsigned int maxSize)=0;
	virtual void destroy()=0;

	virtual void* get()=0;
	virtual void release(void* obj)=0;

	virtual unsigned int available()=0;
	virtual bool changeMaxSize(unsigned int maxSize)=0;

	virtual void setThrowException(bool needThrow)=0;
};

template<class T>
class BLUEMEILIB_TEMPLATE SimpleMemoryPool : public MemoryPool
{
public:
	SimpleMemoryPool(unsigned int itemSize=128,unsigned int maxSize=1024);
	virtual ~SimpleMemoryPool();
public:
	virtual void init(unsigned int itemSize,unsigned int maxSize);
	virtual void destroy();

	virtual void* get();
	virtual void release(void* obj);

	virtual unsigned int available();
	virtual bool changeMaxSize(unsigned int maxSize);

	virtual void setThrowException(bool needThrow);
protected:
	virtual bool growSize(unsigned int size);
protected:
	LinkedList<T*> m_listObject;//用于存取
	LinkedList<T*> m_listOriginalObject;//用于最终释放内存
	unsigned int m_nMaxSize;

	bool m_bNeedThrow;

	CriticalLock m_mutexLock;
};


class BLUEMEILIB_API MemoryPoolManager : public Object
{
public:
	static MemoryPoolManager& instance() {
		static MemoryPoolManager manager;
		return manager;
	}
public:
	template<class T>
	MemoryPool* getMemoryPool(unsigned int itemSize=128,unsigned int maxSize=1024){
		MemoryPool* pool=new SimpleMemoryPool<T>(itemSize,maxSize);
		m_listPool.addToLast(pool);
		return pool;
	}
	void releaseMemoryPool(MemoryPool* pool){
		m_listPool.remove(pool);
		delete pool;
	}
protected:
	LinkedList<MemoryPool*> m_listPool;
};

}//end of namespace bluemei