#pragma once
#include "Thread.h"
#include "Date.h"
#include <functional>
#include <queue>
#include <map>

namespace bluemei{

class BLUEMEILIB_API Message : public Object
{
public:
	Message(){
		init(0,0,0,0,nullptr);
	}
	Message(int id){
		init(0,id,0,0,nullptr);
	}
	Message(int id,int priority){
		init(priority,id,0,0,nullptr);
	}
	Message(int id,int arg1,int arg2){
		init(0,id,arg1,arg2,nullptr);
	}
	Message(int id,int arg1,int arg2,Object* object){
		init(0,id,arg1,arg2,object);
	}
	Message(int id,int priority,int arg1,int arg2,Object* object){
		init(priority,id,arg1,arg2,object);
	}
	virtual ~Message(){}
public:
	void init(int priority,int id,int arg1,int arg2,Object* object){
		m_priority=priority;
		m_id=id;
		m_arg1=arg1;
		m_arg2=arg2;
		m_object=object;
		m_timestamp=Date::getCurrentTime().getTotalMillSecond();
		//static decltype(m_timestamp) count=0;
		//m_timestamp=count++;
	}
	virtual void release(){
		delete m_object;
		m_object=nullptr;
		delete this;
	}
public:
	int getPriority() const { return m_priority; }
	void setPriority(int val) { m_priority = val; }

	int  getId() const { return m_id; }
	void setId(int val) { m_id = val; }

	int  getArg1() const { return m_arg1; }
	void setArg1(int val) { m_arg1 = val; }

	int  getArg2() const { return m_arg2; }
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
};

template<class EleType,class KeyGetter>
class PriorityQueue : public Object
{
	typedef int Key;
	typedef std::queue<EleType> SPQueue;//same Priority Queue	
public:
	PriorityQueue(){};
	virtual ~PriorityQueue(){};
public:	
	bool empty() const
	{	// test if queue is empty
		return (size()==0);
	}

	unsigned int size() const
	{	// return length of queue
		unsigned int count=0;
		for(auto i=m_priorityMap.begin();i!=m_priorityMap.end();++i)
			count += i->second.size();
		return count;
	}

	const EleType& top() const
	{	// return highest-priority element
		//assert(!(m_inerQueue.empty() && !m_priorityMap.empty()));
		Key priority=(m_inerQueue.top());
		SPQueue* spq=getPriorityQueue(priority);
		//assert(spq!=nullptr);
		return spq->front();
	}

	EleType& top()
	{	// return mutable highest-priority element (retained)
		//assert(!(m_inerQueue.empty() && !m_priorityMap.empty()));
		Key priority=(m_inerQueue.top());
		SPQueue* spq=getPriorityQueue(priority);
		//assert(spq!=nullptr);
		return spq->front();
	}

	void push(const EleType& val)
	{	// insert value in priority order
		Key priority=keyGetter(val);
		SPQueue* spq=getPriorityQueue(priority);
		if(spq!=nullptr){
			if(spq->empty())
				m_inerQueue.push(priority);
			spq->push(val);
		}
		else{
			m_inerQueue.push(priority);

			SPQueue q;
			q.push(val);
			m_priorityMap.emplace(make_pair(priority,q));
		}
	}

	void pop()
	{	// erase highest-priority element
		//assert(!(m_inerQueue.empty() && !m_priorityMap.empty()));
		Key priority=(m_inerQueue.top());
		SPQueue* spq=getPriorityQueue(priority);
		//assert(spq!=nullptr);
		spq->pop();
		if(spq->empty())
			m_inerQueue.pop();
	}
	
protected:
	SPQueue* getPriorityQueue(Key priority){
		if(m_priorityMap.count(priority)==1)
			return &m_priorityMap[priority];
		else
			return nullptr;
	}
protected:
	std::priority_queue<Key> m_inerQueue;
	std::map<Key,SPQueue> m_priorityMap;
	KeyGetter keyGetter;
};

typedef std::function<void (Message* msg)> MessageFunction;

class BLUEMEILIB_API MessageThread : public Thread
{
public:
	MessageThread();
	MessageThread(const MessageFunction& fun);
	virtual ~MessageThread();
private:
	MessageThread(const MessageThread& other);
	MessageThread& operator=(const MessageThread& other);
public:
	virtual void addMessage(Message* msg);
	virtual void clearMessage();
	virtual Message* peekMessage();
	virtual Message* nextMessage();
	virtual Message* waitMessage();
	virtual bool hasMessage()const;
public:
	virtual void doMessageLoop();
	virtual void finish();
protected:
	virtual void run(ThreadParameter *pThreadParameter);
protected:
	virtual void onMessage(Message* msg);
protected:
	struct GetKey{
		int operator()(const Message* ele)const{			
			return ele->getPriority();//×î´ó¶Ñ
		}
	};

	PriorityQueue<Message*,GetKey> m_messageQueue;
	MessageFunction m_msgFun;
private:
	MutexLock m_queueLock;
	ResourceLock m_messageLock;
};

}//end of namespace bluemei