#pragma once
#include "stdafx.h"
#include "ObjectList.h"


namespace brpc{

/*
* 函数对象类
* @author Javeme
* @create 2014/7/5
*/
class AnyFunction : public Object
{
public:
	AnyFunction(){}
	virtual ~AnyFunction(){}
public:
	virtual bool operator==(const AnyFunction& other) const 
	{ 
		return this->equals(&other);
	}
public:
	virtual Object* operator()(const ObjectList& args) = 0;
	virtual float matchArgsType(const ObjectList& args) = 0;
	virtual int argsSize() const = 0;
	virtual cstring name() const = 0;
	virtual byte* address() const = 0;

	virtual bool equals(const AnyFunction* other) const = 0;

public:
	static Object* invoke(AnyFunction* func, const ObjectList& args)
	{
		checkNullPtr(func);
		return (*func)(args);
	}

};


}//end of namespace brpc