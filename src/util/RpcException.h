#pragma once
#include "blib.h"


namespace brpc{

/*
* Rpc异常
* @author Javeme
* @create 2013/5/28
*/
class RpcException : public bluemei::Exception
{
public:
	RpcException(void);
	RpcException(cstring str);
	virtual ~RpcException(void);
	String name() const;
};

class RpcCallException : public RpcException
{
public:
	RpcCallException(void);
	RpcCallException(int error, cstring str);
	virtual ~RpcCallException(void);
	String name() const;
	int error()const;
private:
	int err;
};


/*
* 参数不匹配异常类
* @author Javeme
* @create 2014/7/4
*/
class ArgNotMatchedException : public RuntimeException
{
public:
	ArgNotMatchedException(cstring msg) : RuntimeException(msg)
	{
		;
	}

	virtual ~ArgNotMatchedException(void)
	{
	}

	String name()const
	{
		return "ArgNotMatchedException";
	}
};


class AmbiguityFunctionException : public ArgNotMatchedException
{
public:
	AmbiguityFunctionException(cstring msg)
		: ArgNotMatchedException(msg) {}

	virtual ~AmbiguityFunctionException(void) {}

	String name() const
	{
		return "AmbiguityFunctionException";
	}
};

class NotMapException : public RuntimeException
{
public:
	NotMapException(Object* obj)
	{
		checkNullPtr(obj);
		this->setExceptionMsg(String::format("Object '%s' is not a map",
			obj->toString().c_str()));
	}

	virtual String name() const
	{
		return "NotMapException";
	}
};

}//end of namespace brpc