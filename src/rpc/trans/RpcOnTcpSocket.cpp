#pragma once
#include "blib/LambdaThread.h"
#include "blib/ServerSocket.h"
#include "blib/Exceptions.h"
#include "blib/CodeUtil.h"
#include "src/rpc/trans/RpcOnTcpSocket.h"
#include "src/util/BRpcUtil.h"


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
		Log::getLogger()->warn("Failed to close socket: " + e.toString());
	}
	delete this->recvThread;
	delete this->clientSocket;
}

void RpcOnTcpSocket::initSocket(const HashMap<String,String>& paras)
{
	String timeout;
	paras.get("timeout", timeout);
	this->timeout = CodeUtil::str2Int(timeout);
	this->clientSocket->setTimeout(this->timeout);

	String noDelay = "true";
	paras.get("noDelay", noDelay);
	this->clientSocket->setNoDelay(noDelay=="true");

	this->timeoutCount = 0;

	if(!isInServer())
		startReceiveThread();
}

void RpcOnTcpSocket::acceptWith(Object* server,
	const HashMap<String,String>& paras) throw(Exception)
{
	ServerSocket* serverSocket = dynamic_cast<ServerSocket*>(server);
	if (serverSocket)
	{
		this->clientSocket = serverSocket->accept();
		this->inServer = true;
		initSocket(paras);
	}
	else
	{
		throwpe(RpcException("the type of parameter 'server' in "
			"RpcOnTcpSocket::accept() must be ServerSocket"));
	}
}

void RpcOnTcpSocket::connect(const HashMap<String,String>& paras) throw(Exception)
{
	//释放原来的Socket内存
	if(this->clientSocket != null)
	{
		stopReceiveThread();
		delete this->clientSocket;
		this->clientSocket = null;
	}
	//判断是否为服务端
	String server;
	paras.get("server", server);
	if (server == "true"){
		//建立新的连接
		String port;
		paras.get("port", port);
		ServerSocket serverSocket(CodeUtil::str2Int(port));
		this->clientSocket = serverSocket.accept();
		//服务端
		this->inServer = true;
	}
	else{
		//客户端
		String ip,port;
		paras.get("ip", ip);
		paras.get("port", port);
		this->clientSocket = new ClientSocket();
		this->clientSocket->connect(ip, CodeUtil::str2Int(port));
		this->inServer = false;
	}
	initSocket(paras);
}

void RpcOnTcpSocket::close() throw(Exception)
{
	if (this->clientSocket == null)
	{
		throwpe(RpcException("can't close null socket"));
	}

	// stop and wait util next data or timeout
	stopReceiveThread();

	// close TCP socket
	this->clientSocket->close();
}

void RpcOnTcpSocket::startReceiveLoop()
{
	this->recving = true;
	this->hasError = false;
	this->timeoutCount = 0;
	while(this->recving){
		try{
			this->receive();
			this->timeoutCount = 0;
			this->hasError = false;
		}catch(TimeoutException& e){
			notifyException(e);
			this->timeoutCount++;
			if(maxTimeoutCount != 0 && this->timeoutCount > maxTimeoutCount){
				this->hasError = true;
				this->recving = false;
				BRpcUtil::debug("====timeout count > %d, stop reveiver.\n",
					maxTimeoutCount);
			}
		}catch(SocketTryAgainException&){
			BRpcUtil::debug("====exception, try again later.\n");
		}catch(SocketClosedException&){
			// closed by peer (peer -> self)
			this->hasError = false;
			this->recving = false;
			// close self side (self -> peer)
			try{
				this->clientSocket->close();
			}catch(Exception& e){
				Log::getLogger()->warn("Failed to close socket: " +
					e.toString());
			}
		}catch(Exception& e){
			//e.printStackTrace();
			this->hasError = true;
			BRpcUtil::debug("====exception(when receiving): %s\n",
				e.toString().c_str());
			// notify the RpcReceiveListener
			this->recving &= notifyException(e);
		}
	} // end of while

	// notify error, let the notify with HOOK_ERR_RECV_STOPED
	// or HOOK_ERR_CLOSED delete this, and then close this socket
	// please don't do anything after that! (this may be deleted)
	if(this->hasError)
		notifyHookError(toString(), HOOK_ERR_RECV_STOPED);
	else
		notifyHookError(toString(), HOOK_ERR_CLOSED);
}

void RpcOnTcpSocket::stopReceiveLoop()
{
	this->recving=false;
	// interrupt the block read operation by changing the timeout
	if(this->clientSocket!=null)
		this->clientSocket->setTimeout(1);
}

//启动接收数据线程
void RpcOnTcpSocket::startReceiveThread()
{
	if(this->recvThread != null || this->recving)
		return;

	this->recving = true;
	this->recvThread = new LambdaThread([&](){
		//BRpcUtil::debug("====thread started for %s\n", toString().c_str());
		startReceiveLoop();
		//end of thread
	});
	this->recvThread->setAutoDestroy(false);
	this->recvThread->start();
}

void RpcOnTcpSocket::stopReceiveThread()
{
	if(this->recving && this->recvThread != null)
	{
		stopReceiveLoop();
		this->recvThread->wait();
	}
	if(this->recvThread != null)
	{
		delete this->recvThread;
		this->recvThread = null;
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
