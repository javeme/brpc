#pragma once
#include "ClientSocket.h"
//#include "IOCompletionPortImpl.h"
#include "MemoryPool.h"


namespace bluemei{

enum IOCPEventType
{
	EVENT_IN  = 0x0001,			//表示对应的文件描述符可以读；
	EVENT_OUT = 0x0002,			//表示对应的文件描述符可以写；
	EVENT_PRI = 0x0008,			//表示对应的文件描述符有紧急的数据可读
	EVENT_ERR = 0x0010,			//表示对应的文件描述符发生错误；
	EVENT_HUP = 0x0020,			//表示对应的文件描述符被挂断；

	EVENT_LT  = 0x0040,			//表示对应的文件描述符有事件发生；
	EVENT_ET  = 0x0080,			//表示对应的文件描述符有事件发生；

	EVENT_READ		= EVENT_IN,
	EVENT_WRITE		= EVENT_OUT,

	EVENT_READ_FINISH		= 0x0100,
	EVENT_WRITE_FINISH		= 0x0200,
	EVENT_ACCEPT			= 0x0400,
	EVENT_CLOSED			= 0x0800,


	EVENT_FIRST_SOCKET		= 0x80000000,
};

typedef unsigned int __uint32_t;
typedef unsigned long long __uint64_t;


struct IOCPData
{
	const static int IOCP_BUFFER_SIZE=1024;

	OVERLAPPED ol;				   // 重叠结构
	//WSABUF dataBuffer;
	char buf[IOCP_BUFFER_SIZE];	// 接收数据缓冲区
	int operationType;			 // 操作类型
	int para;					   // 其它参数

	unsigned long lengthReceived;
	unsigned long lengthSended;
	unsigned long flags;
};


typedef struct IOEventData { //not union!
	void *ptr;
	int fd;
	__uint32_t u32;
	__uint64_t u64;

	__uint32_t length() const { return u32; }
	byte* bytes() const { return (byte*)((IOCPData*)ptr)->buf; }
}iocp_data;

typedef struct IOEvent {
	__uint32_t events; /* Epoll events */
	IOEventData data; /* User data variable */
}iocp_event;


//class IOCompletionPortImplWin32 : public IOCompletionPortImpl
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

protected:
	void accept(socket_t sock);
	void receive(socket_t sock);
private:
	 HANDLE m_hIOCompletionPort;

	 MemoryPool* m_pPoolIOCPData;
};

}//end of namespace bluemei
