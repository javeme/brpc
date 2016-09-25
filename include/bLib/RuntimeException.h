#pragma once

#include "bluemeiLib.h"
#include "Exception.h"

namespace bluemei{

class BLUEMEILIB_API RuntimeException : public Exception
{
public:
	RuntimeException(){}
	RuntimeException(cstring str);
	virtual ~RuntimeException();
	virtual String name()const;
};


class BLUEMEILIB_API TypeException : public RuntimeException
{
public:
	TypeException(cstring msg);
	TypeException(cstring instance, cstring cls);
	virtual ~TypeException(void){};
public:
	virtual String name()const;
};


class BLUEMEILIB_API BadCastException : public RuntimeException
{
public:
	BadCastException(cstring str);
	BadCastException(cstring from, cstring to);
	virtual ~BadCastException(void){};
public:
	virtual String name()const;
};


class BLUEMEILIB_API NullPointerException : public RuntimeException
{
public:
	NullPointerException(cstring str):RuntimeException(str){};
	virtual ~NullPointerException(void){};
	virtual String name()const{
		return "NullPointerException";
	}
};

template<typename T>
inline void checkNullPtr(T* p) throw(NullPointerException)
{
	if (p==nullptr){
		String str=String::format("instance of type %s* is null",CODE2STRING(T));
		throwpe(NullPointerException(str));
	}
}


class BLUEMEILIB_API NotFoundException : public RuntimeException
{
public:
	NotFoundException(cstring msg);
	virtual ~NotFoundException(void);
	virtual String name()const;
};

class ClassNotFoundException:public NotFoundException
{
public:
	ClassNotFoundException(cstring msg);
	virtual ~ClassNotFoundException(void);
	virtual String name()const;
};

class AttributeNotFoundException:public NotFoundException
{
public:
	AttributeNotFoundException(cstring cls, cstring attr);
	virtual ~AttributeNotFoundException(void);
	virtual String name()const;
};


class BLUEMEILIB_API ExistException : public RuntimeException
{
public:
	ExistException(cstring msg);
	virtual ~ExistException(void);
	virtual String name()const;
};


class BLUEMEILIB_API KeyExistException : public ExistException
{
public:
	KeyExistException(cstring msg);
	virtual ~KeyExistException(void);
	virtual String name()const;
};


class BLUEMEILIB_API OutOfBoundException : public RuntimeException
{
public:
	OutOfBoundException(cstring str):RuntimeException(str){};
	OutOfBoundException(int out,int size);
	virtual ~OutOfBoundException();
	virtual String name()const;
};

class BLUEMEILIB_API OutOfMemoryException : public RuntimeException
{
public:
	OutOfMemoryException(long long size);
	virtual ~OutOfMemoryException(void){};
public:
	virtual String name()const;
};

class BLUEMEILIB_API TimeoutException : public RuntimeException
{
public:
	TimeoutException(long long time);
	virtual ~TimeoutException(void){};
public:
	virtual String name()const;
};

class BLUEMEILIB_API InvalidArgException : public RuntimeException
{
public:
	InvalidArgException(cstring str):RuntimeException(str){};
	virtual ~InvalidArgException(void){};
public:
	virtual String name()const
	{
		return "InvalidArgException";
	}
};


}//end of namespace bluemei