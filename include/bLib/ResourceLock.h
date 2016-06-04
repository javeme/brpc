#pragma once
#include "bluemeiLib.h"
//#include <windows.h>
#include "Exception.h"
#include "Object.h"
#include "MutexLock.h"

namespace bluemei{

#define DEFAULT_MAX_COUNT LONG_MAX
#define waitTask() getLock()

class BLUEMEILIB_API ResourceLock : public Object
{
public:
	ResourceLock(unsigned int count=0,cstring name=nullptr);
	virtual ~ResourceLock(void);
public:
	ResourceLock(const ResourceLock& other);
	ResourceLock& operator=(const ResourceLock& other);
private:
	HANDLE semaphoreHandle;
	volatile unsigned int waitCount;//等待的线程数(跨进程时如何保证waitCount共享???)
protected:
	virtual void init(unsigned int count,unsigned int maxCount,cstring name=nullptr);
public:
	virtual void getLock();
	virtual bool wait(unsigned long mSecond=INFINITE) throw(Exception);
	virtual bool wait(MutexLock& mutex,unsigned long mSecond=INFINITE) throw(Exception);
	virtual long signal() throw(Exception);
	virtual void notify() throw(Exception);
	virtual void notifyAll() throw(Exception);
	virtual long getWaitCount() const;
};
typedef ResourceLock SyncLock;

}//end of namespace bluemei