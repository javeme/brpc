#ifndef _UniqueLock_h_
#define _UniqueLock_h_

#include "bluemeiLib.h"
#include "Object.h"

namespace blib{

class BLUEMEILIB_API UniqueLock : public Object
{
public:
	UniqueLock(){};
	virtual ~UniqueLock(void){};
public:
	virtual void getLock()=0;
	virtual void releaseLock()=0;
};

class BLUEMEILIB_API ScopedLock : Object
{
public:
	explicit ScopedLock(UniqueLock& m): m_mutex(m)
	{
		m_mutex.getLock();
	}
	virtual ~ScopedLock()
	{
		m_mutex.releaseLock();
	}

private:
	UniqueLock& m_mutex;

private:
	ScopedLock();
	ScopedLock(const ScopedLock&);
	ScopedLock& operator = (const ScopedLock&);
};


}//end of namespace blib
#endif