#include "StdAfx.h"
#include "RpcOnHttpSocket.h"
#include "LambdaThread.h"
#include "ServerSocket.h"
#include "Exceptions.h"
#include "CodeUtil.h"
#include "HttpHeader.h"
#include "HttpParser.h"
#include "Log.h"

namespace bluemei{
	
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
void RpcOnHttpSocket::send(const DataPackage& package) throw(IOException)
{
	ScopedLock lock(m_sendLock);

	if (m_pSocket==null){
		throwpe(Exception("null socket"));
	}
	ClientSocket& sock = *m_pSocket;

	const ByteBuffer& output = package.body;
	unsigned int len=output.size();

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

	if(!package.headers.contain(KEY_CONTENT_LEN))
		header->setContentLength(len);
	ByteBuffer headerBuf(header->getEntrySize()*20);
	header->writeTo(headerBuf);
	sock.writeEnoughBytes((const char*)headerBuf.array(),headerBuf.size());//Header
	
	//发送数据
	sock.writeEnoughBytes((const char*)output.array(),output.size());//Data
	//通知数据钩子
	notifyHookSent(sock.getPeerHost(),package);
}

void RpcOnHttpSocket::receive() throw(RpcException)
{
	if(m_pSocket==null){
		throwpe(IOException("null socket"));
	}
	ClientSocket& sock = *m_pSocket;

	//读头部	
	LinkedList<String> lines;
	unsigned int len = 0;
	String line;
	//read util empty line
	while(len = (line = sock.readLine()).length() > 0){
		if(len == sock.LINE_BUFFER_SIZE){
			String err = "The length of a line is too big";
			sendResponse(HttpResponse::RequestEntityTooLarge, "<response-error>");
			throwpe(HttpBadRequestException(err));
		}
		lines.addToLast(line);
		//printf("==%s\n",line.c_str());
	}

	HttpParser parser;
	SmartPtr<HttpHeader> header = parser.parse(lines);

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
		SmartPtr<HttpResponse> resp = header.dynamicCast<HttpResponse>();
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
	int total = 0;
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

void RpcOnHttpSocket::sendResponse(HttpResponse::Status status, const String& pkgId)
{
	DataPackage package;
	package.headers.put(KEY_STATUS, HttpResponse::status2str(status));
	package.headers.put(KEY_PACKAGE_ID, pkgId);
	send(package);
}


}//end of namespace bluemei