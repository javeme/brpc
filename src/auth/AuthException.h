#pragma once
#include "Exception.h"

namespace bluemei{

class AuthException : public Exception
{
public:
	AuthException(void);
	AuthException(cstring str);
	virtual ~AuthException(void);
	String toString() const;
};

class LogoutException : public Exception
{
public:
	LogoutException(void);
	LogoutException(cstring str);
	virtual ~LogoutException(void);
	String toString() const;
};

class LoginException : public Exception
{
public:
	LoginException(void);
	LoginException(cstring authToken, cstring str);
	virtual ~LoginException(void);
	String toString() const;

	String getAuthToken() const { return authToken; }
	void setAuthToken(cstring val) { authToken = val; }
private:
	String authToken;
};

}//end of namespace bluemei