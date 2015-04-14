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

class my_mutex
{
public:
	my_mutex (bool be_initial_owner = false)
	{
		mutex = CreateMutexA (NULL, be_initial_owner, NULL);
	}
	~my_mutex (void)
	{
		CloseHandle (mutex);
	}
public:
	int acquire (void)
	{
		DWORD ret = WaitForSingleObject (mutex, INFINITE);
		return ret == WAIT_OBJECT_0 ? 0 : -1;
	}
	int release (void)
	{
		BOOL bret = ReleaseMutex (mutex);
		return bret ? 0 : -1;
	}
	HANDLE handle (void)
	{
		return mutex;
	}
protected:
	HANDLE mutex;
};

}//end of namespace bluemei
#endif