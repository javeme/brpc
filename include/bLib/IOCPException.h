#pragma once
#include "SocketException.h"
#include "SocketTools.h"
#include "ClientSocket.h"


namespace bluemei{

class BLUEMEILIB_API IOCPException : public SocketException
{
public:
	IOCPException():SocketException("iocp exception"){};
	IOCPException(cstring s,int error=0):SocketException(error,s){};
	IOCPException(int error):SocketException(error){}


	virtual String toString()const{
		return "IOCPException:"+m_strLastError+"("+SocketTools::getWinsockErrorMsg(m_nError)+")";
	}
};

class BLUEMEILIB_API IOCPForceCloseException : public IOCPException
{
public:
	IOCPForceCloseException(socket_t client,int error=0):sock(client),IOCPException("iocp client force close exception",error){};

	virtual String toString()const{
		return "IOCPForceCloseException:"+m_strLastError+"("+SocketTools::getWinsockErrorMsg(m_nError)+")";
	}

	socket_t getSocket()const{
		return sock;
	}
protected:
	socket_t sock;
};

}//end of namespace bluemei
