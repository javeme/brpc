#include "stdafx.h"
#include "RpcOnJmtpSocket.h"
#include "LambdaThread.h"
#include "ServerSocket.h"
#include "Exceptions.h"
#include "CodeUtil.h"
#include "JmtpHeader.h"
#include "Log.h"

namespace brpc{

#define BYTE_ORDER(s) htons(s)
#define JMTP_TAIL 0x00


RpcOnJmtpSocket::RpcOnJmtpSocket(void)
{
	;
}

RpcOnJmtpSocket::~RpcOnJmtpSocket(void)
{
	;
}

//send: header data tail
void RpcOnJmtpSocket::send(const DataPackage& package) throw(IOException)
{
	ScopedLock lock(m_sendLock);

	if (m_pSocket==null){
		throwpe(Exception("null socket"));
	}
	ClientSocket& sock = *m_pSocket;

	const ByteBuffer& output = package.body;
	unsigned int len=output.size();
	if(len>0){
		//发送头部
		//sock.writeShort(BYTE_ORDER(len));//LEN(short) 按小端发送
		//static word count=0;
		//sock.writeShort(BYTE_ORDER(count++));//CTR		
		JmtpHeader header(package.headers);
		if(!package.headers.contain(KEY_CONTENT_LEN))
			header.setContentLength(len);

		ByteBuffer headerBuffer;
		header.writeTo(headerBuffer);
		sock.writeEnoughBytes((const char*)headerBuffer.array(),
			headerBuffer.size());//Header

		//发送数据	
		sock.writeEnoughBytes((const char*)output.array(),output.size());//Data
		//发送尾部
		sock.writeByte(JMTP_TAIL);//Tail

		//通知数据钩子
		notifyHookSent(toString(),package);		
	}
	else
		System::debugInfo("send null data\n");
}

//接收数据
void RpcOnJmtpSocket::receive() throw(RpcException)
{
	if(m_pSocket==null){
		throwpe(IOException("null socket"));
	}
	ClientSocket& sock = *m_pSocket;

	//读头部
	//word len=BYTE_ORDER(sock.readShort());
	//word ctr=BYTE_ORDER(sock.readShort());//TCP本身已经保证有序

	//unsigned char headerBuf[JmtpHeader::HEADER_LEN];
	unsigned int headerLength = JmtpHeader::headerLength();
	ByteBuffer headerBuf(headerLength, headerLength);
	(void)sock.readEnoughBytes((char*)headerBuf.array(), headerLength);
	JmtpHeader header;
	header.readFrom(headerBuf);

	//bad request [in server]
	dword len = header.getContentLength();
	if(isInServer() && len > BRPC_MAX_BODY_LEN){
		String err = "Content Length Too Large";
		sendResponse(JmtpHeader::JMTP_BODY_TOO_LARGE, header.getPackageIdStr());
		throwpe(JmtpException(err));
	}

	//error response [in client]
	if(!isInServer() && header.getStatus() != JmtpHeader::JMTP_OK){
		throwpe(JmtpException(header.getStatusStr()));
	}

	//组装
	DataPackage package;
	package.headers.put(KEY_CONTENT_TYPE, header.getContentTypeStr());
	package.headers.put(KEY_PACKAGE_ID, header.getPackageIdStr());


	//读Data
	ByteBuffer& input = package.body;
	const int LEN = 4096;
	byte buf[LEN];
	dword total = 0;
	while(total < len){
		int wantRead = len - total;
		if(wantRead > LEN)
			wantRead = LEN;
		int read = sock.readBytes((char*)buf, wantRead);
		input.writeBytes(buf, read);
		total += read;
	}

	//读尾部
	byte tail = sock.readByte();
	if (tail != JMTP_TAIL)
	{
		throwpe(IOException("not matched the tail"));
	}
	
	//通知数据钩子
	notifyHookReceived(toString(),package);
	//通知收到数据
	notifyReceive(package);
}

void RpcOnJmtpSocket::sendResponse(JmtpHeader::Status status, const String& pkgId)
{
	DataPackage package;
	package.headers.put(KEY_STATUS, CodeUtil::int2Str(status));
	package.headers.put(KEY_PACKAGE_ID, pkgId);
	send(package);
}

}//end of namespace brpc