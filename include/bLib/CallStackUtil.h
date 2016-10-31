#ifndef CallStackUtil_H_H
#define CallStackUtil_H_H

#include "bluemeiLib.h"
#include "Object.h"
#include "StackWalker.h"
#include "CriticalLock.h"
#include "BString.h"

namespace blib{

class BLUEMEILIB_API CallStackUtil : public StackWalker
{
public:
	static CallStackUtil* inscance();
protected:
	CallStackUtil();
	virtual ~CallStackUtil();
public:
	virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry);
	virtual void OnOutput(LPCSTR szText);

	bool obtainCallStack(List<String>& list);
	//List<String> getCallStackMsgs() const { return m_listCallStackMsg; }
protected:
	List<String>* m_pListCallStackMsg;
	CriticalLock m_lock;
protected:
	static CallStackUtil staticCallStackUtil;
};

}//end of namespace blib
#endif