#ifndef _ClientSocket_h_
#define _ClientSocket_h_

#include "bluemeiLib.h"
#include <winsock2.h>
#include "Object.h"
#include "BString.h"

namespace bluemei{

typedef SOCKET socket_t;

class BLUEMEILIB_API ClientSocket : public Object
{
public:
	ClientSocket();
	ClientSocket(socket_t hSocket);
	~ClientSocket();
public:
	void connect(cstring ip,unsigned short port);

	void setPeerAddr(sockaddr_in& addr);
	virtual String toString()const;

	String getPeerHost()const;//若返回string&是否会出错?
	int getPeerPort()const;

	void setTimeout(int ms)/*超时,毫秒 */;
	void setNoDelay(bool noDelay);

	unsigned long availableBytes();
	void skip(unsigned long len);

	int readBytes(char buffer[],int maxLength,int flags=0);
	int readEnoughBytes(char buffer[],int length);
	int readInt();
	int readShort();
	unsigned char readByte();
	//String& readLineByUtf();
	String readLine();
	//String readUtfString(int wantToReadSize);
	String readUtfString();

	//int writeBytes(byte*& buffer,int length);
	int writeBytes(const char buffer[],int length,int flags=0);
	int writeEnoughBytes(const char buffer[],int length);
	int writeInt(int value);
	int writeShort(short value);
	int writeByte(unsigned char value);
	int writeUtfString(const String& str);
	int writeString(const String& str);

	void close();

public:
	void attach(socket_t s);
	socket_t detach();

protected:
	void createSocket();

protected:
	socket_t m_hSocket;
	sockaddr_in m_peerAddr;
private:
	bool m_bClose;
	int m_nTimeout;
public:
	const static int LINE_BUFFER_SIZE;
};


}//end of namespace bluemei
#endif