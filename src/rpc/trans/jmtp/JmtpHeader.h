#pragma once
#include "Object.h"
#include "ByteBuffer.h"
#include "RpcSocket.h"
#include "RpcSerializerFactory.h"

namespace bluemei{

#define JMTP_NAME "JMTP"
#define JMTP_VERSION 1
	
class JmtpHeader : public Object
{
public:
	enum Status{
		JMTP_OK=0,
		JMTP_ERROR=1,
		JMTP_BAD_ENTITY=3,
		JMTP_BODY_TOO_LARGE=4,
	};
public:
	JmtpHeader(){ init(); }
	JmtpHeader(dword len, word type){ init(len, type); }
	JmtpHeader(const HashMap<String,String>& headers);
public:
	void init(dword len=0, word type=0,
		word counter=0, dword version=JMTP_VERSION,
		word reserv1=0, word reserv2=0);

	virtual void writeTo(ByteBuffer& output)const throw(Exception);
	virtual void readFrom(ByteBuffer& input) throw(Exception);
public:
	static unsigned int headerLength(dword version=JMTP_VERSION);

	static BrpcContentType str2contentType(cstring type);
	static cstring contentType2str(BrpcContentType type);
public:
	word getVersion() const { return version; }
	void setVersion(word val) { version = val; }

	word getStatus() const { return status; }
	void setStatus(word val) { status = val; }
	String getStatusStr() const;	

	dword getContentLength() const { return length; }
	void setContentLength(dword val) { length = val; }

	dword getPackageId() const { return packageId; }
	void setPackageId(dword val) { packageId = val; }
	String getPackageIdStr() const;

	word getCounter() const { return counter; }
	void setCounter(word val) { counter = val; }

	word getContentType() const { return contentType; }
	void setContentType(word val) { contentType = val; }
	cstring getContentTypeStr() const;

	word getReserv1() const { return reserv1; }
	void setReserv1(word val) { reserv1 = val; }

	word getReserv2() const { return reserv2; }
	void setReserv2(word val) { reserv2 = val; }
protected:
	bool checkName() const;
protected:
	const static unsigned int NAME_LEN = 4;
	char name[NAME_LEN];
	word version;
	word status;
	dword length;
	dword packageId;
	word counter;
	word contentType;
	//word encoding;
	word reserv1, reserv2;
};

//JMTP_HEADER_LEN
const unsigned int JMTP_HEADER_LEN = (sizeof(JmtpHeader) - 0) - sizeof(Object);

/*
enum JmtpEncoding
{
	JMTP_ENCODING_UTF8=1,
	JMTP_ENCODING_UNICODE=2,
	JMTP_ENCODING_ASICLL=3,
	JMTP_ENCODING_GBK=4,
	JMTP_ENCODING_GB2312=5,
};*/

}//end of namespace bluemei