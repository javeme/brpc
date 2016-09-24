#include "StdAfx.h"
#include "RpcOnTcpSocket.h"
#include "LambdaThread.h"
#include "ServerSocket.h"
#include "Exceptions.h"
#include "CodeUtil.h"
#include "HttpHeader.h"
#include "HttpParser.h"
#include "Log.h"

namespace brpc{
	
#define PATH_RPC_SERVICE "/rpcservice"

RpcOnTcpSocket::RpcOnTcpSocket(void)
{
	this->clientSocket = null;
	this->recvThread = null;
	this->recving = false;
	this->hasError = false;
	this->inServer = false;

	this->timeoutCount = 0;
}


RpcOnTcpSocket::~RpcOnTcpSocket(void)
{
	try{
		close();
	}catch(Exception& e){			
		System::debugInfo("%s\n",e.toString().c_str());
	}
	delete this->recvThread;
	delete this->clientSocket;
}

void RpcOnTcpSocket::initSocket(const HashMap<String,String>& paras)
{
	String timeout;
	paras.get("timeout",timeout);
	this->timeout = CodeUtil::str2Int(timeout);
	this->clientSocket->setTimeout(this->timeout);

	String noDelay;
	paras.get("noDelay",noDelay);
	this->clientSocket->setNoDelay(noDelay=="true");

	this->timeoutCount = 0;
	
	if(!isInServer())
		startReceiveThread();
}

void RpcOnTcpSocket::acceptWith(Object* server, const HashMap<String,String>& paras) throw(RpcException)
{
	ServerSocket* serverSocket = dynamic_cast<ServerSocket*>(server);
	if (serverSocket)
	{
		this->clientSocket=serverSocket->accept();
		this->inServer = true;
		initSocket(paras);
	}
	else
	{
		throwpe(RpcException("the type of parameter 'server' in "
			"RpcOnTcpSocket::accept() must be ServerSocket"));
	}
}

void RpcOnTcpSocket::connect(const HashMap<String,String>& paras) throw(IOException)
{
	//释放原来的Socket内存
	if(this->clientSocket!=null)
	{
		stopReceiveThread();
		delete this->clientSocket;
		this->clientSocket = null;
	}
	//判断是否为服务端
	String server;
	paras.get("server",server);
	if (server == "true"){
		//建立新的连接
		String port;
		paras.get("port",port);
		ServerSocket serverSocket(CodeUtil::str2Int(port));
		this->clientSocket=serverSocket.accept();
		//服务端
		this->inServer = true;
	}
	else{
		//客户端
		String ip,port;
		paras.get("ip",ip);
		paras.get("port",port);
		this->clientSocket=new ClientSocket();
		this->clientSocket->connect(ip,CodeUtil::str2Int(port));
		this->inServer = false;
	}
	initSocket(paras);
}

void RpcOnTcpSocket::close() throw(IOException)
{
	if (this->clientSocket==null)
	{
		throwpe(Exception("null socket"));
	}

	stopReceiveThread(); //wait util next data or timeout 
	this->clientSocket->close();
}

void RpcOnTcpSocket::startReceiveLoop()
{
	this->recving = true;
	this->timeoutCount = 0;
	while(this->recving){
		try{
			receive();
			this->timeoutCount = 0;
			this->hasError = false;
		}catch(TimeoutException& e){
			notifyException(e);
			this->timeoutCount++;
			if(this->timeoutCount > maxTimeoutCount){
				this->hasError = true;
				this->recving = false;
				BRpcUtil::debug("====timeout count > %d, stop reveiver.",
					maxTimeoutCount);
			}
		}catch(SocketClosedException&){
			this->hasError = false;
			this->recving = false;
		}catch(Exception& e){
			//e.printStackTrace();
			this->hasError = true;
			BRpcUtil::debug("====exception(when receiving): %s\n",
				e.toString().c_str());
			//通知异常
			this->recving &= notifyException(e);
		}
	}//end while
	/*try{
		m_pSocket->close();
	}catch(Exception& e){			
		System::debugInfo("%s\n",e.toString().c_str());
	}*/
	if(this->hasError)
		notifyHookError(toString(), HOOK_ERR_RECV_STOPED);
	else
		notifyHookError(toString(), HOOK_ERR_CLOSED);
}

void RpcOnTcpSocket::stopReceiveLoop()
{
	this->recving=false;
	if(this->clientSocket!=null)
		this->clientSocket->setTimeout(1);
}

//启动接收数据线程
void RpcOnTcpSocket::startReceiveThread()
{
	if(this->recvThread != null || this->recving)
		return;

	this->recvThread=new LambdaThread([&](){
		//BRpcUtil::debug("====thread started for %s\n", toString().c_str());
		startReceiveLoop();
		//end of thread
	},nullptr);
	this->recvThread->setAutoDestroy(false);
	this->recvThread->start();
}

void RpcOnTcpSocket::stopReceiveThread()
{
	if(this->recving && this->recvThread!=null)
	{
		stopReceiveLoop();
		this->recvThread->wait();
	}
}

bool RpcOnTcpSocket::isRecving() const
{
	return this->recving;
}

bool RpcOnTcpSocket::isAlive() const
{
	return this->recving && !this->hasError;
}

bool RpcOnTcpSocket::isInServer() const
{
	return this->inServer;
}

String RpcOnTcpSocket::toString() const
{
	if (this->clientSocket == null)
		return "<null>";
	else
		return this->clientSocket->toString();
}

}//end of namespace brpc