#pragma once
#include "bluemeiLib.h"
#include "Thread.h"
#include <functional>

namespace blib{

typedef std::function<void (void)> ThreadFunction;

class BLUEMEILIB_API LambdaThread : public Thread {
public:
	LambdaThread(const ThreadFunction& f,void* pUserParameter=NULL);
	virtual ~LambdaThread();
	virtual void run();
private:
	ThreadFunction m_lambdaFun;
};

#if 0
//test
void download(string url,const function<void (int error)>& f)//f为下载完成后的回调函数
{
	auto func=[url,f](){
		printf("downloading...\n");
		Sleep(1000*5);
		printf("download finished from %s\n",url.c_str());
		f(0);
	};
	//thread
	Thread *pThread=new LambdaThread(func,nullptr);//下载线程
	pThread->start();
}
#endif

}//end of namespace blib