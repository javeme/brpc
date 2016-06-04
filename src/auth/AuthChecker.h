#pragma once
#include "AuthException.h"
#include "ObjectList.h"


namespace brpc{

class AuthChecker : public Object
{
public:
	virtual bool checkAuth(cstring authToken, cstring method)=0;
	virtual String login(const ObjectList& args)=0;
	virtual bool logout(cstring authToken)=0;
};

}//end of namespace brpc