#pragma once
#include "blib.h"


namespace brpc{

class Number;
class ObjectList;
class ObjectMap;
class TypeIterator;

class TypeVisiter : public bluemei::Object
{
public:
	TypeVisiter(){
		;
	}
	virtual ~TypeVisiter(){
		;
	}
public:
	virtual void visit(Number* v)=0;
	virtual void visit(String* v)=0;
	virtual void visit(ObjectList* v)=0;
	virtual void visit(ObjectMap* v)=0;
	virtual void visit(TypeIterator* v)=0;
};

class TypeVisitable/* : public virtual Object*/
{
public:
	virtual ~TypeVisitable(){}
public:
	virtual void accept(TypeVisiter* visitor)=0;
};

class TypeIterator : public TypeVisitable
{
public:
	virtual ~TypeIterator(){}
public:
	virtual void accept(TypeVisiter* visitor){
		checkNullPtr(visitor);
		visitor->visit(this);
	}
};

/*
class Visitor : public Object
{
public:
	virtual void visit(Object* ele)=0;
};

class Visitable : public Object
{
public:
	virtual void accept(Visitor* visitor)=0;
}
;*/

}//end of namespace brpc

