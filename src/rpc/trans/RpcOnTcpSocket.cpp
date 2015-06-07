#include "StdAfx.h"
#include "RpcOnTcpSocket.h"
#include "LambdaThread.h"
#include "ServerSocket.h"
#include "Exceptions.h"
#include "CodeUtil.h"
#include "HttpHeader.h"
#include "HttpParser.h"
#include "Log.h"

namespace bluemei{
	
#define PATH_RPC_SERVICE "/rpcservice"

RpcOnTcpSocket::RpcOnTcpSocket(void)
{
	m_pSocket = null;
	m_pRecvThread = null;
	m_bRecving = false;
	m_bError = false;
	m_bInServer = false;

	m_nTimeoutCount = 0;
}


RpcOnTcpSocket::~RpcOnTcpSocket(void)
{
	try{
		close();
	}catch(Exception& e){			
		System::debugInfo("%s\n",e.toString().c_str());
	}
	delete m_pRecvThread;
	delete m_pSocket;
}

void RpcOnTcpSocket::initSocket(const HashMap<String,String>& paras)
{
	String timeout;
	paras.get("timeout",timeout);
	this->timeout = CodeUtil::str2Int(timeout);
	m_pSocket->setTimeout(this->timeout);

	String noDelay;
	paras.get("noDelay",noDelay);
	m_pSocket->setNoDelay(noDelay=="true");

	m_nTimeoutCount = 0;
	startReceiveThread();
}

void RpcOnTcpSocket::acceptWith(Object* server, const HashMap<String,String>& paras) throw(RpcException)
{
	ServerSocket* serverSocket = dynamic_cast<ServerSocket*>(server);
	if (serverSocket)
	{
		m_pSocket=serverSocket->accept();
		m_bInServer = true;
		initSocket(paras);
	}
	else
	{
		throwpe(RpcException("the type of parameter 'server' in RpcOnTcpSocket::accept() must be ServerSocket"));
	}
}

void RpcOnTcpSocket::connect(const HashMap<String,String>& paras) throw(IOException)
{
	//释放原来的Socket内存
	if(m_pSocket!=null)
	{
		stopReceiveThread();
		delete m_pSocket;
		m_pSocket = null;
	}
	//判断是否为服务端
	String server;
	paras.get("server",server);
	if (server == "true"){
		//建立新的连接
		String port;
		paras.get("port",port);
		ServerSocket serverSocket(CodeUtil::str2Int(port));
		m_pSocket=serverSocket.accept();
		//服务端
		m_bInServer = true;
	}
	else{
		//客户端
		String ip,port;
		paras.get("ip",ip);
		paras.get("port",port);
		m_pSocket=new ClientSocket();
		m_pSocket->connect(ip,CodeUtil::str2Int(port));
		m_bInServer = false;
	}
	initSocket(paras);
}

void RpcOnTcpSocket::close() throw(IOException)
{
	if (m_pSocket==null)
	{
		throwpe(Exception("null socket"));
	}

	stopReceiveThread();//wait util next data or timeout 
	m_pSocket->close();
	
	/*m_pSocket->close();
	stopReceiveThread();*/
}

//启动接收数据线程
void RpcOnTcpSocket::startReceiveThread()
{
	if(m_pRecvThread != null || m_bRecving)
		return;

	m_pRecvThread=new LambdaThread([&](){
		//BRpcUtil::debug("====thread started for %s\n", toString().c_str());
		m_bRecving = true;
		m_nTimeoutCount = 0;
		while(m_bRecving){
			try{
				receive();
				m_nTimeoutCount = 0;
				m_bError = false;
			}catch(TimeoutException& e){
				notifyException(e);
				m_nTimeoutCount++;
				if(m_nTimeoutCount > m_nMaxTimeoutCount){
					m_bError = true;
					m_bRecving = false;
					BRpcUtil::debug("====timeout count > %d, stop reveiver.", m_nMaxTimeoutCount);
				}
			}catch(SocketClosedException&){
				m_bError = false;
				m_bRecving = false;
			}catch(Exception& e){
				//e.printStackTrace();
				m_bError = true;
				BRpcUtil::debug("====exception(when receiving): %s\n", e.toString().c_str());
				//通知异常
				m_bRecving &= notifyException(e);
			}
		}//end while
		/*try{
			m_pSocket->close();
		}catch(Exception& e){			
			System::debugInfo("%s\n",e.toString().c_str());
		}*/
		if(m_bError)
			notifyHookError(toString(), HOOK_ERR_RECV_STOPED);
		else
			notifyHookError(toString(), HOOK_ERR_CLOSED);
		//end of thread
	},nullptr);
	m_pRecvThread->setAutoDestroy(false);
	m_pRecvThread->start();
}

void RpcOnTcpSocket::stopReceiveThread()
{
	if(m_bRecving && m_pRecvThread!=null)
	{
		m_bRecving=false;
		if(m_pSocket!=null)
			m_pSocket->setTimeout(1);
		m_pRecvThread->wait();
	}
}

bool RpcOnTcpSocket::isRecving() const
{
	return m_bRecving;
}

bool RpcOnTcpSocket::isAlive() const
{
	return isRecving() && !m_bError;
}

bool RpcOnTcpSocket::isInServer() const
{
	return m_bInServer;
}

String RpcOnTcpSocket::toString() const
{
	if (m_pSocket == null)
		return "<null>";
	else
		return m_pSocket->toString();
}

}//end of namespace bluemei