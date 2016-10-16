/*
*edit by bluemei
*write for java-users
*/
#pragma once
#ifndef _Thread_h_
#define _Thread_h_

#include "bluemeiLib.h"
#include "Runnable.h"
#include "ThreadException.h"
#include "ResourceLock.h"
#include "CriticalLock.h"

namespace bluemei{

class BLUEMEILIB_API Thread : public Runnable
{
public:
	Thread(void* pUserParameter=NULL);;
	Thread(Runnable* pTarget,void* pUserParameter=NULL);
	virtual ~Thread();
private:
	Thread(const Thread& other);
	Thread& operator=(const Thread& other);
public:
	virtual void start() throw(Exception);
	virtual void stop();
	virtual void attach();
	virtual void detach();
	virtual void wait() throw(ThreadException);
	static void sleep(unsigned int msecond);

	void setAutoDestroy(bool bAutoDestroy);
	bool isAutoDestroyObj() const;
	void* getUserParameter() const;
public:
	virtual void run();
	virtual bool isRunning() const;
	virtual unsigned int getThreadId() const;
	static unsigned int currentThreadId();
	static unsigned int mainThreadId();
public:
	int callBackStartThread();
protected:
	virtual void init();
	virtual void destroy();
protected:
	Runnable* m_pObject;
	void* m_pUserParameter;
	unsigned int m_threadId;
	static unsigned int s_mainThreadId;

	SyncLock m_lock;
	bool m_bAutoDestroyObj;
	volatile bool m_bRunning,m_bDetached;
private:
	HANDLE m_hThread;
};

}//end of namespace bluemei

#endif