#pragma once
#include "src/rpc/trans/jmtp/JmtpHeader.h"


namespace brpc{

struct ContentEncodingMapIniter
{
	Map<JmtpHeader::Encoding, String> contentEncoding2StrMap;
	Map<String, JmtpHeader::Encoding> str2ContentEncodingMap;

	ContentEncodingMapIniter()
	{
		#define ce2s(k, v) contentEncoding2StrMap[JmtpHeader::k] = v
		ce2s(JMTP_ENCODING_BIN, "bin");
		ce2s(JMTP_ENCODING_UTF8, "utf8");
		ce2s(JMTP_ENCODING_UNICODE, "unicode");
		ce2s(JMTP_ENCODING_ASICLL, "asicll");
		ce2s(JMTP_ENCODING_GBK, "gbk");
		ce2s(JMTP_ENCODING_GB2312, "gb2312");

		for(auto i = contentEncoding2StrMap.begin();
			i != contentEncoding2StrMap.end(); i++)
		{
			str2ContentEncodingMap[i->second] = i->first;
		}
	}

	JmtpHeader::Encoding str2ContentEncoding(cstring encoding) const
	{
		JmtpHeader::Encoding result = JmtpHeader::JMTP_ENCODING_BIN;
		auto itor = str2ContentEncodingMap.find(String(encoding).toLower());
		if(itor != str2ContentEncodingMap.end())
			result = itor->second;
		return result;
	}

	cstring contentEncoding2Str(JmtpHeader::Encoding encoding) const
	{
		auto itor = contentEncoding2StrMap.find(encoding);
		if(itor != contentEncoding2StrMap.end())
			return itor->second;
		return "";
	}
};

static ContentEncodingMapIniter s_contentEncodingMapIniter;


JmtpHeader::JmtpHeader(const HashMap<String,String>& headers)
{
	int len = CodeUtil::str2Int(headers.getDefault(KEY_CONTENT_LEN, "0"));
	String contentType = headers.getDefault(KEY_CONTENT_TYPE, "");
	String contentEncoding = headers.getDefault(KEY_CHARSET, "");
	String packageId = headers.getDefault(KEY_PACKAGE_ID, "");

	init((dword)len, JmtpHeader::str2ContentType(contentType),
		JmtpHeader::str2ContentEncoding(contentEncoding));

	setPackageId(CodeUtil::str2Int(packageId));//hashCode<String>(packageId)
}

void JmtpHeader::init(dword len/*=0*/, word type/*=0*/,
	word encoding/*=0*/, dword timestamp/*=0*/,
	word counter/*=0*/, dword version/*=JMTP_VERSION*/,
	word reserv1/*=0*/, word reserv2/*=0*/)
{
	(void)memcpy(name, JMTP_NAME, NAME_LEN);
	//this->name[NAME_LEN] = '\0';
	this->version = version;
	this->status = JMTP_OK;
	this->packageId = 0;
	this->counter = counter;
	this->timestamp = timestamp;

	this->contentType = type;
	this->contentEncoding = encoding;
	this->contentLength = len;

	this->reserv1 = reserv1;
	this->reserv2 = reserv2;

	if (timestamp == 0) {
		long long current = Date::getCurrentTime().getTotalMillSecond();
		this->timestamp = (dword)(current / 1000); // ms => second
	}
}

bool JmtpHeader::checkName() const
{
	return memcmp(name, JMTP_NAME, NAME_LEN) == 0;
}

void JmtpHeader::writeTo(ByteBuffer& output) const throw(Exception)
{
	// 1
	output.writeBytes((const byte*)name, NAME_LEN);
	// 2
	output.writeShort(headerLength());
	output.writeShort(version);
	// 3
	output.writeShort(status);
	output.writeShort(counter);
	// 4
	output.writeInt(packageId);
	// 5
	output.writeInt(timestamp);
	// 6
	output.writeShort(contentType);
	output.writeShort(contentEncoding);
	// 7
	output.writeInt(contentLength);
	// 8
	output.writeShort(reserv1);
	output.writeShort(reserv2);
}

void JmtpHeader::readFrom(ByteBuffer& input) throw(Exception)
{
	// 1
	input.readBytes((byte*)name, NAME_LEN);
	if(!checkName())
		throw IOException("not matched the jmtp");
	// 2
	headerSize = input.readShort();
	version = input.readShort();
	// 3
	status = input.readShort();
	counter = input.readShort();
	// 4
	packageId = input.readInt();
	// 5
	timestamp = input.readInt();
	// 6
	contentType = input.readShort();
	contentEncoding = input.readShort();
	// 7
	contentLength = input.readInt();
	// 8
	reserv1 = input.readShort();
	reserv2 = input.readShort();
}

unsigned int JmtpHeader::headerLength(dword version)
{
	if(version == JMTP_VERSION)
		return JMTP_HEADER_LEN;
	/*else if(version == 1)
		return JMTP_V1_HEADER_LEN;*/
	else
		return JMTP_HEADER_LEN;
}

BrpcContentType JmtpHeader::str2ContentType(cstring type)
{
	return RpcSerializerFactory::str2contentType(type);
}

cstring JmtpHeader::contentType2Str(BrpcContentType type)
{
	return RpcSerializerFactory::contentType2str(type);
}

cstring JmtpHeader::getContentTypeStr() const
{
	return contentType2Str((BrpcContentType)contentType);
}

JmtpHeader::Encoding JmtpHeader::str2ContentEncoding(cstring encoding)
{
	return s_contentEncodingMapIniter.str2ContentEncoding(encoding);
}

cstring JmtpHeader::contentEncoding2Str(Encoding encoding)
{
	return s_contentEncodingMapIniter.contentEncoding2Str(encoding);
}

cstring JmtpHeader::getContentEncodingStr() const
{
	return contentEncoding2Str((Encoding)contentEncoding);
}

String JmtpHeader::getPackageIdStr() const
{
	return String::format("%u", packageId);
}

String JmtpHeader::getStatusStr() const
{
	switch(status)
	{
	case JMTP_OK:
		return "Jmtp ok";
	case JMTP_ERROR:
		return "Jmtp error";
	case JMTP_BAD_ENTITY:
		return "Jmtp bad entity";
	case JMTP_BODY_TOO_LARGE:
		return "Jmtp body too large";
	default:
		return "Jmtp unknow status";
	}
}

}//end of namespace brpc