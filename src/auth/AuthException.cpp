#pragma once
#include "src/auth/AuthException.h"


namespace brpc{

/*****************************************************************/
AuthException::AuthException(void):Exception("Authority Exception")
{
}

AuthException::AuthException(cstring str):Exception(str)
{
}

AuthException::~AuthException(void)
{
}

String AuthException::toString() const
{
	return "AuthException : "+m_strLastError;
}


/*****************************************************************/
LogoutException::LogoutException(void):Exception("Logout Exception")
{
}

LogoutException::LogoutException(cstring str):Exception(str)
{
}

LogoutException::~LogoutException(void)
{
}

String LogoutException::toString() const
{
	return "LogoutException : "+m_strLastError;
}


/*****************************************************************/
LoginException::LoginException(void):Exception("Login Exception")
{
}

LoginException::LoginException(cstring authToken, cstring str)
	: authToken(authToken), Exception(str)
{
}

LoginException::~LoginException( void )
{
}

String LoginException::toString() const
{
	return "LoginException : "+m_strLastError;
}

}//end of namespace brpc