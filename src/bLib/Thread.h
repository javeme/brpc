/*
*edit by bluemei
*write for java-users
*/

#ifndef _Thread_h_
#define _Thread_h_

#include "bluemeiLib.h"
//#include <windows.h>

#include "ThreadParameter.h"
#include "Runnable.h"
#include "ThreadException.h"
#include "ResourceLock.h"
#include "CriticalLock.h"

namespace bluemei{

class BLUEMEILIB_API Thread : public Runnable
{
public:
	//用于 内部类或派生类 (c++内部类无法直接访问外部变量和方法,所以也无法用于内部类.确实需要的话可以传参数,不过就不如合并了)
	Thread(void* pUserParameter=NULL){this->Thread::Thread(this,pUserParameter);};	
	Thread(Runnable* pTarget,void* pUserParameter=NULL);//用于接口
	//Thread(ThreadParameter *pThreadParameter);//pThreadParameter must be created with operater 'new'
	Thread(const ThreadParameter& threadParameter);
	virtual ~Thread();
private:
	Thread(const Thread& other);
	Thread& operator=(const Thread& other);
protected:
	ThreadParameter m_threadParameter;
	HANDLE m_hThread;
	volatile bool m_bRunning,m_bDetached;
	bool m_bAutoDestroyObj;
	SyncLock m_lock;
	unsigned int m_threadId;
	//CriticalLock m_criticalLock;
public:
	void setAutoDestroy(bool bAutoDestroy);
	void start() throw(Exception);
	void stop();
	void detach();
	void wait() throw(ThreadException);
	static void sleep(unsigned int msecond);
public:
	bool isRunning() const;
	unsigned int getThreadId() const;
	static unsigned int currentThreadId();
public: 
	void callBackStartThread();
	virtual void run(ThreadParameter *pThreadParameter);
protected: 
	virtual void init();
	virtual void destroy();
};
unsigned int WINAPI callBackOfStartThread(void* pVoid);


}//end of namespace bluemei
#endif