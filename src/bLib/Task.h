/*
package test.bluemei.task;

public interface Task extends Runnable{	
	public void beforeTask();
	public void taskFinish();
	public int getId();
	public void setId(int id);
}
*/
#pragma once
#include "Runnable.h"

namespace bluemei{

class BLUEMEILIB_API Task:public Runnable
{
public:
	//virtual void run(ThreadParameter *pThreadParameter)=0;
	virtual void stop()=0;
public:
	virtual bool isRunning()=0;
	virtual void beforeTask()=0;
	virtual void taskFinish()=0;
	virtual int getId()=0;
	virtual void setId(int id)=0;
};

}//end of namespace bluemei