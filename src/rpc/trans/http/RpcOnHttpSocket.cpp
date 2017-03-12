#pragma once
#include "blib/LambdaThread.h"
#include "blib/ServerSocket.h"
#include "blib/Exceptions.h"
#include "blib/CodeUtil.h"
#include "src/rpc/trans/http/RpcOnHttpSocket.h"
#include "src/rpc/trans/http/HttpHeader.h"
#include "src/rpc/trans/http/HttpParser.h"


namespace brpc{

#define PATH_RPC_SERVICE "/rpcservice"

RpcOnHttpSocket::RpcOnHttpSocket(void)
{
	;
}

RpcOnHttpSocket::~RpcOnHttpSocket(void)
{
	;
}

//send: header data tail
void RpcOnHttpSocket::send(const DataPackage& package) throw(Exception)
{
	ScopedLock lock(this->sendLock);

	if (this->clientSocket == null){
		throwpe(Exception("null socket"));
	}
	ClientSocket& sock = *this->clientSocket;

	const ByteBuffer& output = package.body;
	unsigned int len = output.size();

#ifdef DEBUG_SEG_BUG
	// Bug: segment error 11
	//发送头部
	SmartPtr<HttpHeader> header = null;
	//package.headers.getDefault(KEY_RESPONSE, "false") == "true"
	bool isResponse = this->isInServer();
	if (isResponse){
		header = SmartPtr<HttpResponse>(new HttpResponse(package.headers));
		//header->addCookie("sessionId", "abcd-2234-dddd-cccc-bbbb-aaaa");
	}
	else{
		SmartPtr<HttpRequest> request = new HttpRequest(package.headers);
		request->setRequestType(HttpRequest::HTTP_POST);
		request->setRequestUrl(PATH_RPC_SERVICE);
		header = request;
	}
#else
	//发送头部
	ScopePointer<HttpHeader> header = null;
	//package.headers.getDefault(KEY_RESPONSE, "false") == "true"
	bool isResponse = this->isInServer();
	if (isResponse){
		header = new HttpResponse(package.headers);
		//header->addCookie("sessionId", "abcd-2234-dddd-cccc-bbbb-aaaa");
	}
	else{
		ScopePointer<HttpRequest> request = new HttpRequest(package.headers);
		request->setRequestType(HttpRequest::HTTP_POST);
		request->setRequestUrl(PATH_RPC_SERVICE);
		header = request.detach();
	}
#endif

	if(!package.headers.contain(KEY_CONTENT_LEN))
		header->setContentLength(len);
	ByteBuffer headerBuf(header->getEntrySize()*20);
	header->writeTo(headerBuf);

	// send HTTP Header
	sock.writeEnoughBytes((const char*)headerBuf.array(),headerBuf.size());
	// send HTTP Data
	sock.writeEnoughBytes((const char*)output.array(),output.size());

	//通知数据钩子
	notifyHookSent(sock.getPeerHost(), package);
}

void RpcOnHttpSocket::receive() throw(Exception)
{
	if(this->clientSocket==null){
		throwpe(IOException("null socket"));
	}
	ClientSocket& sock = *this->clientSocket;

	//读头部
	LinkedList<String> lines;
	unsigned int len = 0;
	String line;
	//read util empty line
	while((len = (line = sock.readLine()).length()) > 0){
		if(len >= (unsigned int)sock.LINE_BUFFER_SIZE){
			String err = "The length of a line is too big";
			sendResponse(HttpResponse::RequestEntityTooLarge, "<response-error>");
			throwpe(HttpBadRequestException(err));
		}
		lines.addToLast(line);
		//printf("==%s\n",line.c_str());
	}

	HttpParser parser;
	ScopePointer<HttpHeader> header = std::move(parser.parse(lines));

	//bad request [in server]
	bool isRequest = this->isInServer();
	String packageId = header->geEntity(KEY_PACKAGE_ID, "");
	len = header->getContentLength();
	if(isRequest){
		if(len == 0){
			String err = "'Content-Length' Required";
			sendResponse(HttpResponse::LengthRequired, packageId);
			throwpe(HttpBadRequestException(err));
		}
		else if(len > BRPC_MAX_BODY_LEN){
			String err = "'Content-Length' Too Large";
			sendResponse(HttpResponse::RequestEntityTooLarge, packageId);
			throwpe(HttpBadRequestException(err));
		}
	}
	//error response [in client]
	else{
		HttpResponse* resp = dynamic_cast<HttpResponse*>((HttpHeader*)header);
		if(resp == null){
			throwpe(HttpException("Invalid Http Response"));
		}
		else if(resp->getStatus() != HttpResponse::Ok){
			throwpe(HttpException(resp->status2str(resp->getStatus())));
		}
	}

	//组装
	DataPackage package;
	package.headers = header->getHeaders();
	package.headers.put(KEY_CONTENT_TYPE, header->getContentType());
	package.headers.put(KEY_CHARSET, header->getCharset());

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
	/*for (int i=0; i<len; i++){
		input.put(sock.readByte());
	}*/

	/*Log::getLogger()->debug(String::format(
		">>>>>>>>>>received: headers==%s, body==%s",
		package.headers.toString().c_str(),
		String((cstring)package.body.array(), package.body.size()).c_str()));*/

	//通知数据钩子
	notifyHookReceived(toString(),package);
	//通知收到数据
	notifyReceive(package);
}

void RpcOnHttpSocket::sendResponse(HttpResponse::Status status,
		const String& pkgId) throw(Exception)
{
	DataPackage package;
	package.headers.put(KEY_STATUS, HttpResponse::status2str(status));
	package.headers.put(KEY_PACKAGE_ID, pkgId);
	send(package);
}


}//end of namespace brpc
