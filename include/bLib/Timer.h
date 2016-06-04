#pragma once
#include "Class.h"
#include "ResourceLock.h"
#include "LambdaThread.h"

namespace bluemei{

class BLUEMEILIB_API Timer : public Object
{
public:
	Timer();
	Timer(Class& taskClass,unsigned long delay,unsigned long period=0);
	virtual ~Timer();
public:
	//void schedule(Class* pTaskClass,unsigned long delay,unsigned long period=0);//Runnable *pTask
	virtual void schedule(Class& taskClass,unsigned long delay,unsigned long period=0);
	virtual void schedule(Runnable* task,unsigned long delay,unsigned long period,bool autoDestroyTask);
	virtual bool isRunning()const;
	virtual void cancel();
	virtual void reset();//重置定时器, 线程不安全方法,请慎用!!!
private:
	void executeTask();//Runnable * pTask, unsigned long delay
	long getRemainderTime();
private:
	Runnable* m_pTask;
	unsigned long m_nPeriod;
	unsigned long m_nextTime;
	volatile bool m_bGoOn;
	SyncLock m_lock,m_waitFinishLock;
};

}//end of namespace bluemei