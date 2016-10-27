#pragma once
#include "WorkThread.h"
#include "LinkedList.h"
#include <queue>
using std::queue;
#define Queue queue

namespace bluemei{

#define TaskPool ThreadPool

class BLUEMEILIB_API ThreadPool : public Object
{
public:
	ThreadPool(int threadCount=5);
	~ThreadPool();
public:
	static ThreadPool* getInstance(int threadCount=5);
	static void destroy(ThreadPool* pThreadPool);

protected:
	SyncLock m_taskLock;
	CriticalLock m_mutexLock;

	LinkedList<WorkThread*> m_threadList;
	//LinkedList<WorkThread*> doingThreadList;
	Queue<Task*> m_taskQueue;

	volatile bool m_bIsStop;
	int m_nThreadCount;
private:
	void addThread(int threadCount);
public:
	int addTask(Task *pTask);
	Task* getTask();
	void onFinishTask(Task* pTask);
	void stop();
	int addMoreThread(int num);
public:
	int remainTask();
	int totalThread() const;
	int idleThread();
	bool isStop() const;
};

}//end of namespace bluemei