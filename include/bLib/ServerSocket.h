#ifndef _ServerSocket_h_
#define _ServerSocket_h_

#include <winsock2.h>
#include <string>

#include "bluemeiLib.h"
#include "ClientSocket.h"


namespace blib{

class BLUEMEILIB_API ServerSocket : public Object
{
public:
	ServerSocket();
	ServerSocket(int nPort);
	~ServerSocket();
protected:
	socket_t m_socket;//¼àÌýÌ×½Ó×Ö
private:
	unsigned short m_sPort;
	bool m_bClose;
public:
	ClientSocket* accept();
	void listen(int nPort);
	void close();//¹Ø±Õ
public:
	operator socket_t() const;

	static void destroy(ClientSocket*& pClientSocket);
protected:
	int createSocket(int nPort);
};

}//end of namespace blib
#endif
