#pragma once
#include "bluemeiLib.h"
#include "Object.h"


namespace bluemei{

class BLUEMEILIB_API PointerReference : public Object
{
public:
	PointerReference(void);
	virtual ~PointerReference(void);
private:	
	PointerReference(const PointerReference&);
	PointerReference&  operator=(const PointerReference&);
public:
	void attach();
	PointerReference* disattach();
protected:
	int m_nPtrRefCount;
};


template<typename T> 
class BLUEMEILIB_TEMPLATE ScopePointer : public Object
{
public:
	ScopePointer(T* pObj=null)
	{
		targetObj = pObj;
	}
		
	virtual ~ScopePointer(void)
	{
		delete targetObj;
	}

private:
	ScopePointer(const ScopePointer& other);
	ScopePointer& operator = (const ScopePointer& other) const;

public:
	int operator==(T* p) const {
		return this->targetObj == p;
	}

	operator T* () const {
		return targetObj;
	}

	T* operator->() const {
		return targetObj;
	}

	T* detach() {
		T* tmp = targetObj;
		targetObj = null;
		return tmp;
	}

protected:
	T* targetObj;
};

}//end of namespace bluemei