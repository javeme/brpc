#pragma once
#include "bluemeiLib.h"
#include "Object.h"


namespace bluemei{

class BLUEMEILIB_API PointerReference : virtual public Object
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
class BLUEMEILIB_TEMPLATE Pointer : public Object
{
public:
	Pointer(T* target=null) : m_target(target) {}
	Pointer(const Pointer& other) : m_target(other.m_target) {}

public:
	virtual bool operator==(T* target) const {
		return this->m_target == target;
	}

	virtual bool operator!=(T* target) const {
		return this->m_target != target;
	}

	virtual bool operator==(const Pointer& other) const {
		return m_target == other.m_target;
	}

	virtual bool operator!=(const Pointer& other) const {
		return m_target != other.m_target;
	}

	operator T* () const {
		return m_target;
	}

	T* operator->() const {
		return m_target;
	}

	T* detach() {
		T* tmp = m_target;
		m_target = null;
		return tmp;
	}

protected:
	T* m_target;
};


//Wrapper for PointerReference
template<typename T>
class BLUEMEILIB_TEMPLATE RefPointer : public Pointer<T>
{
public:
	RefPointer(T* target) : Pointer(target) {
		if(m_target != null)
			m_target->attach();
	}

	RefPointer(const RefPointer& other) : Pointer(other) {
		if(m_target != null)
			m_target->attach();
	}

	virtual ~RefPointer() {
		if(m_target != null)
			m_target->disattach();
	}

	RefPointer& operator=(const RefPointer& other) {
		if(m_target != null)
			m_target->disattach();
		m_target = other.m_target;
		if (m_target != null)
			m_target->attach();
		return *this;
	}
};


template<typename T>
class BLUEMEILIB_TEMPLATE ScopePointer : public Pointer<T>
{
public:
	ScopePointer(T* target=null) : Pointer(target) {}

	virtual ~ScopePointer(void)
	{
		delete m_target;
	}

private:
	ScopePointer(const ScopePointer& other);
	ScopePointer& operator = (const ScopePointer& other) const;
};

}//end of namespace bluemei