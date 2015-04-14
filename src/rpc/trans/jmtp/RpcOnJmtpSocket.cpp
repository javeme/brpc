#include "stdafx.h"
#include "RpcOnJmtpSocket.h"
#include "LambdaThread.h"
#include "ServerSocket.h"
#include "Exceptions.h"
#include "CodeUtil.h"
#include "JmtpHeader.h"
#include "Log.h"

namespace bluemei{

#define BYTE_ORDER(s) htons(s)
#define JMTP_TAIL 0x00


RpcOnJmtpSocket::RpcOnJmtpSocket(void)
{
	m_pSocket=null;
	m_pRecvThread=null;
	m_bRecving=false;
}


RpcOnJmtpSocket::~RpcOnJmtpSocket(void)
{
	try{
		close();
	}catch(Exception& e){			
		System::debugInfo("%s\n",e.toString().c_str());
	}
	delete m_pRecvThread;
	delete m_pSocket;
}

void RpcOnJmtpSocket::connect( const HashMap<String,String>& paras ) throw(IOException)
{
	//释放原来的Socket内存
	if(m_pSocket!=null)
	{
		delete m_pSocket;
	}
	//判断是否为服务端
	String server;
	paras.get("server",server);
	if (server == "true"){
		//服务端
		//建立新的连接
		String port;
		paras.get("port",port);
		ServerSocket serverSocket(CodeUtil::str2Int(port));
		m_pSocket=serverSocket.accept();
	}
	else{
		//客户端
		String ip,port;
		paras.get("ip",ip);
		paras.get("port",port);
		m_pSocket=new ClientSocket();
		m_pSocket->connect(ip,CodeUtil::str2Int(port));
	}

	String timeout;
	paras.get("timeout",timeout);
	m_pSocket->setTimeout(CodeUtil::str2Int(timeout));//1000*4

	String noDelay;
	paras.get("noDelay",noDelay);
	m_pSocket->setNoDelay(noDelay=="true");

	startReceiveThread();
}

void RpcOnJmtpSocket::close() throw(IOException)
{
	if (m_pSocket==null)
	{
		throwpe(Exception("null socket"));
	}
	/*stopReceiveThread();
	m_pSocket->close();*/
	
	m_pSocket->close();
	stopReceiveThread();
}

//send: header data tail
void RpcOnJmtpSocket::send(const DataPackage& package) throw(IOException)
{
	m_sendLock.getLock();

	if (m_pSocket==null){
		throwpe(Exception("null socket"));
	}

	const ByteBuffer& output = package.body;
	unsigned int len=output.size();
	if(len>0){
		//发送头部
		//m_pSocket->writeShort(BYTE_ORDER(len));//LEN(short) 按小端发送
		//static word count=0;
		//m_pSocket->writeShort(BYTE_ORDER(count++));//CTR		
		JmtpHeader header(package.headers);
		if(!package.headers.contain(KEY_CONTENT_LEN))
			header.setContentLength(len);

		ByteBuffer headerBuffer;
		header.writeTo(headerBuffer);
		m_pSocket->writeEnoughBytes((const char*)headerBuffer.array(),
			headerBuffer.size());//Header

		//发送数据	
		m_pSocket->writeEnoughBytes((const char*)output.array(),output.size());//Data
		//通知数据钩子
		notifyHookSent(m_pSocket->getPeerHost(),package);
		
		//发送尾部
		m_pSocket->writeByte(JMTP_TAIL);//Tail
	}
	else
		System::debugInfo("send null data\n");

	m_sendLock.releaseLock();
}

//启动接收数据线程
void RpcOnJmtpSocket::startReceiveThread()
{
	if(m_pRecvThread!=null || m_bRecving)
		return;

	m_pRecvThread=new LambdaThread([&](void* pUserParameter){
		m_bRecving=true;
		while(m_bRecving){
			try{
				if(m_pSocket==null)
				{
					throwpe(IOException("null socket"));
				}
				//读头部
				//word len=BYTE_ORDER(m_pSocket->readShort());
				//word ctr=BYTE_ORDER(m_pSocket->readShort());//TCP本身已经保证有序
				
				//unsigned char headerBuf[JmtpHeader::HEADER_LEN];
				unsigned int headerLength = JmtpHeader::headerLength();
				ByteBuffer headerBuf(headerLength, headerLength);
				(void)m_pSocket->readEnoughBytes((char*)headerBuf.array(), headerLength);
				JmtpHeader header;
				header.readFrom(headerBuf);
				//组装
				DataPackage package;
				word len = header.getContentLength();
				package.headers.put(KEY_CONTENT_TYPE, header.getContentTypeStr());
				package.headers.put(KEY_PACKAGE_ID, header.getPackageIdStr());
				
				//读Data
				ByteBuffer& input = package.body;
				for (int i=0; i<len; i++)
				{
					input.put(m_pSocket->readByte());
				}

				//读尾部
				byte tail = m_pSocket->readByte();
				if (tail != JMTP_TAIL)
				{
					throwpe(IOException("not matched the tail"));
				}

				Log::getLogger()->debug(String::format(
					">>>>>>>>>>received: headers====%s, body====%s",
					package.headers.toString().c_str(), 
					String((cstring)package.body.array(), package.body.size()).c_str()));

				//通知数据钩子
				notifyHookReceived(m_pSocket->getPeerHost(),package);
				//通知收到数据
				notifyReceive(package);

			}catch(TimeoutException& e){
				notifyException(e);
			}catch(Exception& e){
				//通知异常
				m_bRecving&=notifyException(e);
			}
		}//end while
		/*try{
			m_pSocket->close();
		}catch(Exception& e){			
			System::debugInfo("%s\n",e.toString().c_str());
		}*/
		//end of thread
	},nullptr);
	m_pRecvThread->setAutoDestroy(false);
	m_pRecvThread->start();
}

void RpcOnJmtpSocket::stopReceiveThread()
{
	if(m_bRecving && m_pRecvThread!=null)
	{
		m_bRecving=false;
		m_pRecvThread->wait();
	}
}

bool RpcOnJmtpSocket::isRecving() const
{
	return m_bRecving;
}

bool RpcOnJmtpSocket::isAlive() const
{
	return isRecving();
}


}//end of namespace bluemei