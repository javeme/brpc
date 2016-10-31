#pragma once
#include "blib/BString.h"
#include "src/type/TypeVisitor.h"

namespace brpc{

class TString : public String, public TypeVisitable
{
public:
	TString():String(){};
	TString(cstring src , int len=-1):String(src, len){};
	TString(const String &src):String(src){};
public:
	virtual void accept(TypeVisiter* visitor)
	{
		checkNullPtr(visitor);
		visitor->visit(this);
	}
};

}//end of namespace brpc