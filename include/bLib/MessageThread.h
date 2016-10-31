#pragma once
#include "bluemeiLib.h"
#include "Thread.h"
#include "Date.h"
#include "PriorityQueue.h"
#include <functional>
#include <queue>
#include <map>

namespace blib{


typedef class BLUEMEILIB_API Message : public Object
{
public:
	// message priority
	virtual int getPriority() const = 0;
	// message type
	virtual int getType() const = 0;
} ThreadMessage;


typedef class BLUEMEILIB_API SimpleMessage : public Message
{
public:
	SimpleMessage(){
		init(0,0,0,0,nullptr);
	}
	SimpleMessage(int id){
		init(0,id,0,0,nullptr);
	}
	SimpleMessage(int id,int priority){
		init(priority,id,0,0,nullptr);
	}
	SimpleMessage(int id,int arg1,int arg2){
		init(0,id,arg1,arg2,nullptr);
	}
	SimpleMessage(int id,int arg1,int arg2,Object* object){
		init(0,id,arg1,arg2,object);
	}
	SimpleMessage(int id,int priority,int arg1,int arg2,Object* object){
		init(priority,id,arg1,arg2,object);
	}
	virtual ~SimpleMessage(){}
public:
	void init(int priority,int id,int arg1,int arg2,Object* object){
		m_priority=priority;
		m_id=id;
		m_arg1=arg1;
		m_arg2=arg2;
		m_object=object;
		m_timestamp=Date::getCurrentTime().getTotalMillSecond();
	}
	virtual void release(){
		delete m_object;
		m_object=nullptr;
		delete this;
	}
public:
	int getPriority() const { return m_priority; }
	void setPriority(int val) { m_priority = val; }

	int getId() const { return m_id; }
	void setId(int val) { m_id = val; }
	int getType() const { return getId(); }	

	int getArg1() const { return m_arg1; }
	void setArg1(int val) { m_arg1 = val; }

	int getArg2() const { return m_arg2; }
	void setArg2(int val) { m_arg2 = val; }

	Object* getObject() const { return m_object; }
	void setObject(Object* val) { m_object = val; }

	unsigned long long getTimestamp() const { return m_timestamp; }
	void setTimestamp(unsigned long long val) { m_timestamp = val; }
protected:
	int m_priority;
	int m_id;
	int m_arg1,m_arg2;
	Object* m_object;
	unsigned long long m_timestamp;
} SimpleThreadMessage;


typedef std::function<void (Message* msg)> MessageFunction;

class BLUEMEILIB_API MessageThread : public Thread
{
public:
	MessageThread();
	MessageThread(const MessageFunction& fun);
	virtual ~MessageThread();

	class ExitLoopException : public Exception
	{
	public:
		virtual String name() const {
			return "ExitLoopException";
		}
	};
private:
	MessageThread(const MessageThread& other);
	MessageThread& operator=(const MessageThread& other);
public:
	virtual void postMessage(Message* msg);
	virtual void clearMessage();
	virtual Message* peekMessage();
	virtual Message* nextMessage();
	virtual Message* waitMessage();
	virtual bool hasMessage() const;

	virtual void stop();
	virtual void run();
protected:
	virtual void doMessageLoop();
	virtual void onMessage(Message* msg);
protected:
	struct GetKey{
		int operator()(const Message* ele) const{
			return ele->getPriority();//×î´ó¶Ñ
		}
	};

	PriorityQueue<Message*,GetKey> m_messageQueue;
	MessageFunction m_msgFun;
private:
	MutexLock m_queueLock;
	ResourceLock m_messageLock;
};

}//end of namespace blib