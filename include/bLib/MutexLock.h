#ifndef _MutexLock_h_
#define _MutexLock_h_

#include "bluemeiLib.h"
#include "Exception.h"
#include "UniqueLock.h"

namespace bluemei{

class BLUEMEILIB_API MutexLock : public UniqueLock
{
public:
	MutexLock(bool initialOwner=false,cstring name=nullptr);
	virtual ~MutexLock(void);
private:
	MutexLock(const MutexLock& other);
	MutexLock& operator=(const MutexLock& other);
private:
	friend class ResourceLock;
	HANDLE mutex;
	volatile unsigned int waitCount;//等待的线程数(跨进程时无效)
public:
	virtual void getLock();
	virtual void releaseLock();
	void notify(){ return releaseLock();}
	virtual long getWaitCount() const;
};

}//end of namespace bluemei
#endif