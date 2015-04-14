#pragma once
#include "bluemeiLib.h"
#include "Object.h"

namespace bluemei{

class BLUEMEILIB_API ReferencePointer : public Object
{
public:
	ReferencePointer(void);
	virtual ~ReferencePointer(void);
private:	
	ReferencePointer(const ReferencePointer&);
	ReferencePointer&  operator=(const ReferencePointer&);
public:
	void attach();
	ReferencePointer* disattach();
protected:
	int m_nPtrRefCount;
};



}//end of namespace bluemei