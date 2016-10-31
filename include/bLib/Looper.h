#pragma once
#include "MessageThread.h"
#include "LambdaThread.h"

namespace blib{

class BLUEMEILIB_API Looper : public Object
{
public:
	enum MsgType{
		MSG_EXIT_LOOP,
		MSG_EXE_RUNNABLE,
		MSG_MAX,
	};
public:
	Looper() : m_msgThread(null) {}
	virtual ~Looper(){ delete m_msgThread; }
public:
	/* start message loop
	 * para `newThread` bool:
	 *   true: start a new thread to handle message
	 *   false: run message loop in this thread
	 */
	virtual void start(bool newThread=true);
	// stop message loop
	virtual void stop();
public:
	virtual void postRunnable(const ThreadFunction& f, int priority=0);
protected:
	void onMessage(Message* msg);
protected:
	MessageThread* m_msgThread;
};

}//end of namespace blib