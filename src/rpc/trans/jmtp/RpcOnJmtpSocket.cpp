#pragma once
#include "blib/LambdaThread.h"
#include "blib/ServerSocket.h"
#include "blib/Exceptions.h"
#include "blib/CodeUtil.h"
#include "src/rpc/trans/jmtp/RpcOnJmtpSocket.h"
#include "src/rpc/trans/jmtp/JmtpHeader.h"


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

//send package: header + data + tail
void RpcOnJmtpSocket::send(const DataPackage& package) throw(Exception)
{
	ScopedLock lock(this->sendLock);

	if (this->clientSocket==null){
		throwpe(Exception("null socket"));
	}
	ClientSocket& sock = *this->clientSocket;

	const ByteBuffer& output = package.body;
	unsigned int len = output.size();
	if(len>0){
		// construct header
		JmtpHeader header(package.headers);
		if(!package.headers.contain(KEY_CONTENT_LEN))
			header.setContentLength(len);

		// send header
		ByteBuffer headerBuffer;
		header.writeTo(headerBuffer);
		sock.writeEnoughBytes((const char*)headerBuffer.array(),
			headerBuffer.size());
		// send body
		sock.writeEnoughBytes((const char*)output.array(), output.size());
		// send tail
		sock.writeByte(JMTP_TAIL);

		// notify the data hook
		notifyHookSent(toString(), package);
	}
	else
		System::debugInfo("send null data\n");
}

/*
// TODO: support async read like IOCP
void RpcOnJmtpSocket::receive(ByteBuffer& buffer)
{
	buffer.lock()
		buffer.peek(header, HEADER_LEN);
	int bodyLen = header.getContentLength();
	// header + body
	if(buffer.avail() >= HEADER_LEN + bodyLen) {
		// consume the header(peek before)
		buffer.skip(HEADER_LEN);
		buffer.read(body, bodyLen);
	}
	buffer.unlock()
}
*/

void RpcOnJmtpSocket::receive() throw(Exception)
{
	if(this->clientSocket==null){
		throwpe(IOException("null socket"));
	}
	ClientSocket& sock = *this->clientSocket;

	// read header
	// TODO: read the header length from socket data
	unsigned int headerLength = JmtpHeader::headerLength();
	ByteBuffer headerBuf(headerLength, headerLength);
	(void)sock.readEnoughBytes((char*)headerBuf.array(), headerLength);
	JmtpHeader header;
	header.readFrom(headerBuf);

	// bad request [on server side]
	dword len = header.getContentLength();
	if(isInServer() && len > BRPC_MAX_BODY_LEN){
		String err = "Content Length Too Large";
		sendResponse(JmtpHeader::JMTP_BODY_TOO_LARGE, header.getPackageIdStr());
		throwpe(JmtpException(err));
	}

	// error response [on client side]
	if(!isInServer() && header.getStatus() != JmtpHeader::JMTP_OK){
		throwpe(JmtpException(header.getStatusStr()));
	}

	// assemble package header
	DataPackage package;
	package.headers.put(KEY_CONTENT_TYPE, header.getContentTypeStr());
	package.headers.put(KEY_CHARSET, header.getContentEncodingStr());
	package.headers.put(KEY_PACKAGE_ID, header.getPackageIdStr());

	// read body data
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

	// read tail
	byte tail = sock.readByte();
	if (tail != JMTP_TAIL)
	{
		throwpe(IOException("not matched the tail"));
	}

	// notify the data hook
	notifyHookReceived(toString(),package);
	// handle data received
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
