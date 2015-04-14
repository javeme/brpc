#ifndef _CriticalLock_h_
#define _CriticalLock_h_

#include "bluemeiLib.h"
#include "UniqueLock.h"

namespace bluemei{

class BLUEMEILIB_API CriticalLock : public UniqueLock
{
public:
	CriticalLock();
	virtual ~CriticalLock(void);
public:
	CriticalLock(const CriticalLock& other){
		waitCount=0;
		::InitializeCriticalSection(&critialSection);
		this->operator=(other);
	};
	CriticalLock& operator=(const CriticalLock& other){
		return *this;
	};
private:
	CRITICAL_SECTION critialSection;
	volatile unsigned int waitCount;//等待的线程数
public:
	virtual void getLock();
	virtual void releaseLock();
	void notify(){ return releaseLock();}
	virtual long getWaitCount() const;
	virtual long getMyThreadEnteredCount()const;
};

}//end of namespace bluemei
#endif