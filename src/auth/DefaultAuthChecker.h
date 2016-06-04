#pragma once
#include "AuthException.h"
#include "AuthChecker.h"
#include "HashMap.h"

namespace brpc{

class User : public Object
{
public:
	User(cstring name="", cstring password="")
	{
		this->username = name;
		this->password = password;
	}
	virtual ~User()
	{
	}
public:
	String username;
	String password;
	String authToken;
	String session;
};

class DefaultAuthChecker : public AuthChecker
{
public:
	DefaultAuthChecker(cstring name, cstring password);
	virtual ~DefaultAuthChecker();

	virtual bool checkAuth(cstring authToken, cstring method);
	virtual String login(const ObjectList& args);
	virtual bool logout(cstring authToken);
public:
	virtual bool addUser(cstring name, cstring password);
protected:
	virtual bool addAuthToken(const String& authToken, const String& name);
private:
	HashMap<String, User> userMap;//username,User
	HashMap<String, String> authMap;//auth,username
};

}//end of namespace brpc