#pragma once
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include "ClientSocket.h"
//#include "IOCompletionPortImpl.h"


namespace blib{

#define MAX_FDS 256

enum IOCPEventType
{
	EVENT_IN  = EPOLLIN,			//表示对应的文件描述符可以读；
	EVENT_OUT = EPOLLOUT,			//表示对应的文件描述符可以写；
	EVENT_PRI = EPOLLPRI,			//表示对应的文件描述符有紧急的数据可读
	EVENT_ERR = EPOLLERR,			//表示对应的文件描述符发生错误；
	EVENT_HUP = EPOLLHUP,			//表示对应的文件描述符被挂断；

	EVENT_LT  = EPOLLLT,			//表示对应的文件描述符有事件发生；
	EVENT_ET  = EPOLLET,			//表示对应的文件描述符有事件发生；

	EVENT_READ		= EVENT_IN,
	EVENT_WRITE		= EVENT_OUT,

	EVENT_READ_FINISH		= 0x0100,
	EVENT_WRITE_FINISH		= 0x0200,
	EVENT_ACCEPT			= 0x0400,
	EVENT_CLOSED			= 0x0800,


	EVENT_FIRST_SOCKET		= 0x80000000,
};

typedef int socket_t;
typedef epoll_event IOEvent;


class BLUEMEILIB_API IOCompletionPortImpl : public Object
{
public:
	IOCompletionPortImpl();
	virtual ~IOCompletionPortImpl();

public:
	void registerEvents(int events,socket_t socket);
	void unregisterEvents(int events,socket_t socket);
	void modifyEvents(int events,socket_t socket);

	int waitEvent(IOEvent* events,int maxEvents,int timeout);
	bool cancelWait();
	void releaseEventBuffer(IOEvent* event);

	void send(const byte* buffer, unsigned int len, socket_t sock);
public:
	void create();
	void close();
private:
	int m_hIOCompletionPort;
};

}//end of namespace blib