#pragma once
#include "stdafx.h"
#include "JmtpHeader.h"

namespace brpc{


JmtpHeader::JmtpHeader(const HashMap<String,String>& headers)
{
	int len = CodeUtil::str2Int(headers.getDefault(KEY_CONTENT_LEN, "0"));
	String contentType = headers.getDefault(KEY_CONTENT_TYPE, "");
	String packageId = headers.getDefault(KEY_PACKAGE_ID, "");
	init((dword)len, JmtpHeader::str2contentType(contentType));
	setPackageId(CodeUtil::str2Int(packageId));//hashCode<String>(packageId)
}

void JmtpHeader::init(dword len/*=0*/, word type/*=0*/,
	word counter/*=0*/, dword version/*=JMTP_VERSION*/,
	word reserv1/*=0*/, word reserv2/*=0*/)
{
	(void)memcpy(name, JMTP_NAME, NAME_LEN);
	//this->name[NAME_LEN] = '\0';
	this->version = version;
	this->status = JMTP_OK;
	this->length = len;
	this->packageId = 0;
	this->counter = counter;
	this->contentType = type;
	this->reserv1 = reserv1;
	this->reserv2 = reserv2;
}

bool JmtpHeader::checkName() const
{
	return memcmp(name, JMTP_NAME, NAME_LEN) == 0;
}

void JmtpHeader::writeTo(ByteBuffer& output) const throw(Exception)
{
	output.writeBytes((const byte*)name, NAME_LEN);
	output.writeShort(version);
	output.writeShort(status);
	output.writeInt(length);
	output.writeInt(packageId);
	output.writeShort(counter);
	output.writeShort(contentType);
	output.writeShort(reserv1);
	output.writeShort(reserv2);
}

void JmtpHeader::readFrom(ByteBuffer& input) throw(Exception)
{
	input.readBytes((byte*)name, NAME_LEN);
	if(!checkName())
		throw IOException("not matched the jmtp");
	version = input.readShort();
	status = input.readShort();
	length = input.readInt();
	packageId = input.readInt();
	counter = input.readShort();
	contentType = input.readShort();
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

BrpcContentType JmtpHeader::str2contentType(cstring type)
{
	return RpcSerializerFactory::str2contentType(type);
}

cstring JmtpHeader::contentType2str(BrpcContentType type)
{
	return RpcSerializerFactory::contentType2str(type);
}

cstring JmtpHeader::getContentTypeStr() const
{
	return contentType2str((BrpcContentType)contentType);
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