#pragma once
#include "blib.h"


namespace brpc{

class VarContext : public Object
{
public:
	VarContext(){}
	virtual ~VarContext(void){}	
public:
	virtual bool registerVar(cstring name, Object* var) = 0;
	virtual bool unregisterVar(cstring name) = 0;
	virtual Object* getVarFromAll(cstring name) const = 0;
};

class ObjectRef : public Object
{
public:
	ObjectRef(cstring id, VarContext* context = null) : m_objId(id), m_context(context)
	{
		;
	}

	virtual ~ObjectRef(void)
	{
	}

	String toString()const
	{
		return String::format("<%s>%s", objectType(), getObjectId());
	}

	cstring objectType()const
	{
		Object* obj = getObject();
		if (obj != null)
		{
			return obj->thisClass()->getName();
		}
		return "null";
	}

	virtual cstring getObjectId() const { return m_objId.c_str(); }
	virtual void setObjectId(cstring val) { m_objId = val; }

	virtual Object* getObject() const
	{
		if (m_context != null)
		{
			return m_context->getVarFromAll(m_objId);
		}
		return null;
	}

	virtual VarContext* getContext() const { return m_context; }
	virtual void setContext(VarContext* val) { m_context = val; }
protected:
	String m_objId;
	VarContext* m_context;
};

}//end of namespace bluemei