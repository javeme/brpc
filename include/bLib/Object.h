#ifndef Object_H_H
#define Object_H_H
#include "bluemeiLib.h"

namespace bluemei{

class Class;
class String;
class FieldInfo;

typedef const char* cstring;
typedef unsigned char byte;

#define null nullptr

//∂‘œÛ¿‡
class BLUEMEILIB_API Object
{
public:
	/*class SuperObj{public: static const Class* thisClass(){return nullptr;}};
	#define __super SuperObj
	DECLARE_DCLASS(Object);
	#undef __super*/
	static const Class* thisClass();
	static Object* createObject();
	virtual Object* clone() const;
	virtual const Class* getThisClass() const;
	virtual void setAttribute(cstring name, Object* val);
	virtual Object* getAttribute(cstring name) const;
	virtual const void* getAttributeAddr(cstring name) const;
public:
	Object();
	virtual ~Object();

	virtual void init(Object *pObj);
	virtual void release();
	virtual String toString() const;
};


}//end of namespace bluemei
#endif