#pragma once
#include "Thread.h"
#include "Log.h"
#include "Task.h"
#include "ResourceLock.h"

namespace bluemei{

class ThreadPool;

class BLUEMEILIB_API WorkThread :public Thread
{
public:
	WorkThread(ThreadPool*const pTaskPool);
	~WorkThread(void);
private:
	//Log log;
	//SyncLock *const m_pLock;//m_pLock常指针,内容可变,本身不变
	ThreadPool* const m_pTaskPool;
	Task *m_pCurrentTask;
	volatile bool m_bIsIdle;
public:
	virtual void run();
public:
	virtual void stopAndWait();
	virtual bool isIdle() const;
};


}//end of namespace bluemei