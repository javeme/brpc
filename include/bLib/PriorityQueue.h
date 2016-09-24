#pragma once

#include "bluemeiLib.h"
#include "Exceptions.h"
#include <queue>
#include <map>

namespace bluemei{

//模板类无法被dll导出 BLUEMEILIB_API
template<class EleType,class KeyGetter>
class BLUEMEILIB_TEMPLATE PriorityQueue : public Object
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
		Key priority=(m_inerQueue.top());
		SPQueue* spq=getPriorityQueue(priority);
		//assert(spq!=nullptr);
		if(spq==nullptr)
			throwpe(NullPointerException(String("Not exist priority : ")+priority));
		return spq->front();
	}

	EleType& top()
	{	// return mutable highest-priority element (retained)
		Key priority=(m_inerQueue.top());
		SPQueue* spq=getPriorityQueue(priority);
		//assert(spq!=nullptr);
		if(spq==nullptr)
			throwpe(NullPointerException(String("Not exist priority : ")+priority));
		return spq->front();
	}

	void push(const EleType& val)
	{	// insert value in priority order
		Key priority=m_keyGetter(val);
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
		Key priority=(m_inerQueue.top());
		SPQueue* spq=getPriorityQueue(priority);
		//assert(spq!=nullptr);
		if(spq==nullptr)
			throwpe(NullPointerException(String("Not exist priority : ")+priority));
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
	KeyGetter m_keyGetter;
};

}//end of namespace bluemei