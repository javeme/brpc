#pragma once
#include "stdafx.h"
#include "DefaultAuthChecker.h"
#include "Date.h"

namespace brpc{


DefaultAuthChecker::DefaultAuthChecker(cstring name, cstring password)
{
	(void)addUser(name, password);
}

DefaultAuthChecker::~DefaultAuthChecker()
{

}

bool DefaultAuthChecker::checkAuth(cstring authToken, cstring method)
{
	return authMap.contain(authToken);
}

String DefaultAuthChecker::login(const ObjectList& args)
{
	if (args.size() < 2)
		throw AuthException("Failed to login, need args: 'username' and 'password'");

	String name, password;
	args.getValue(0, name);
	args.getValue(1, password);
	User* user = userMap.get(name);
	if (user != null){
		if (user->password == password) {
			String authToken = name + Util::uuid4();
			user->authToken = authToken;
			bool success = addAuthToken(authToken, name);
			return authToken;
		}
	}
	throw AuthException("Login failed, username or password is error");
}

bool DefaultAuthChecker::logout(cstring authToken)
{
	String username;
	if(authMap.get(authToken, username)) {
		User* user = userMap.get(username);
		if (user != null){
			user->authToken = "";
			user->session = "";
		}
		return true;
	}
	return false;
}

bool DefaultAuthChecker::addUser(cstring name, cstring password)
{
	return userMap.put(name, User(name, password));
}

bool DefaultAuthChecker::addAuthToken(const String& authToken, const String& name)
{
	return authMap.put(authToken, name);
}


}//end of namespace brpc