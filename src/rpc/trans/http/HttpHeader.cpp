#pragma once
#include "stdafx.h"
#include "HttpHeader.h"
#include "CodeUtil.h"
#include "HttpException.h"

namespace brpc{

// Mon, 19 Nov 2014 21:49:59 GMT
#define DATE_FORMAT_GMT "%a, %d %b %Y %H:%M:%S GMT"	
#define CRLF "\r\n"

HttpHeader::HttpHeader(const HashMap<String,String>& headers)
{
	int len = CodeUtil::str2Int(headers.getDefault(KEY_CONTENT_LEN, "0"));
	String contentType = headers.getDefault(KEY_CONTENT_TYPE, "text/json");
	
	this->entities = headers;

	init((dword)len, contentType);
	if (headers.contain(KEY_CHARSET)) {
		setCharset(headers.getDefault(KEY_CHARSET, "UTF-8"));
	}
}

void HttpHeader::init(dword len/*=0*/, cstring type/*=0*/,
	cstring version/*=JMTP_VERSION*/)
{
	this->name = HTTP_NAME;
	this->version = version;
	this->contentLen = len;
	this->contentType = type;
	this->charset = "UTF-8";

	this->setDate(Date::getCurrentTime());
}

bool HttpHeader::checkName() const
{
	return name == HTTP_NAME;
}

void HttpHeader::writeEntitiesTo(OutputStream& output) throw(Exception)
{
	//if (!contain(KEY_CONTENT_TYPE))
	setEntity(KEY_CONTENT_TYPE, getContentTypeAndCharset());
	if (!contain(KEY_CONTENT_LEN))
		setEntity(KEY_CONTENT_LEN, CodeUtil::int2Str(contentLen));
	
	auto itor = entities.iterator();
	while(itor->hasNext())
	{
		auto entry = itor->next();
		output.writeBytes((byte*)entry.key.c_str(), entry.key.length());
		output.writeChar(':');
		output.writeChar(' ');
		output.writeBytes((byte*)entry.value.c_str(), entry.value.length());
		writeCrlfTo(output);
	}
	entities.releaseIterator(itor);

	//cookies
	this->writeCookiesTo(output);
}

void HttpHeader::readEntitiesFrom(InputStream& input) throw(Exception)
{
	String line;
	while((line/* = input.readLine()*/).length() > 0)
	{
		int offset = line.find(":");
		if(offset<=0)
			continue;
		String key = line.getLeftSub(offset);
		String val = line.getRightSub(line.length() - offset -1);
		setEntity(key, val);
	}
	//cookies
	readCookiesFrom(input);
}

BrpcContentType HttpHeader::str2contentType(cstring type)
{
	return RpcSerializerFactory::str2contentType(type);
}

cstring HttpHeader::contentType2str(BrpcContentType type)
{
	return RpcSerializerFactory::contentType2str(type);
}

cstring HttpHeader::getContentType() const
{
	return contentType;
}

void HttpHeader::setContentType(cstring val) 
{
	String str = val;
	auto list = str.splitWith(";");
	contentType = list[0].trim();

	//like this: "Content-Type:text/html; charset=UTF-8" ?
	if(list.size() > 1 && 
		(list = list[1].splitWith("="))[0].trim() == "charset"){
		setCharset(list.size() > 1 ? list[1].trim() : "GBK");
	}
}	

String HttpHeader::getContentTypeAndCharset() const
{
	return contentType + "; charset=" + charset;
}

HashMap<String,String> HttpHeader::getHeaders() const
{
	HashMap<String,String> headers = entities;
	headers.put(KEY_HTTP_VERSION, version);
	headers.put(KEY_CONTENT_TYPE, getContentTypeAndCharset());
	headers.put(KEY_CONTENT_LEN, CodeUtil::int2Str(contentLen));
	return headers;
}

void HttpHeader::update()
{
	setContentLength(CodeUtil::str2Int(entities.getDefault(KEY_CONTENT_LEN, "0")));
	setContentType(entities.getDefault(KEY_CONTENT_TYPE, ""));
}

String HttpHeader::geEntity(const String& key, const String& default)const
{
	return this->entities.getDefault(key, default);
}

void HttpHeader::setEntity(const String& key, const String& val)
{
	(void)this->entities.put(key, val);
}


bool HttpHeader::addCookie(const Cookie& val)
{
	return cookies.put(val.name, val);
}

bool HttpHeader::addCookie(const String& name, const String& val)
{
	return cookies.put(name, Cookie(name, val));
}

String HttpHeader::getCookieAsString(const String& name)const
{
	#define _c CodeUtil::urlEncodeComponent
	const Cookie* cookie = cookies.get(name);
	checkNullPtr(cookie);
	return String::format("%s=%s;Expires=%s;Domain=%s;Path=%s;%s",
		_c(cookie->name).c_str(), _c(cookie->value).c_str(), 
		(cookie->expires.formatDate(DATE_FORMAT_GMT)).c_str(),
		_c(cookie->domain).c_str(), _c(cookie->path).c_str(),
		cookie->secure?"Secure":"");
}

String HttpHeader::getContentEncoding() const
{
	return this->geEntity("Content-Encoding", "");
}

void HttpHeader::setContentEncoding( const String& val )
{
	return this->setEntity("Content-Encoding", val);
}

dword HttpHeader::getKeepAlive() const
{
	return (dword)CodeUtil::str2Int(this->geEntity("Keep-Alive", "0"));
}

void HttpHeader::setKeepAlive( dword sec )
{
	if (sec > 0)
	{
		this->setEntity("Connection", "keepalive");
		this->setEntity("Keep-Alive", CodeUtil::int2Str(sec));
	}
	else
	{
		this->setEntity("Connection", "close");
	}
}

String HttpHeader::getConnection() const
{
	if (getKeepAlive() > 0)
		return "keepalive";
	else
		return "close";
}

String HttpHeader::getDateString() const
{
	return this->geEntity("Date", "");
}

void HttpHeader::setDate( const String& date )
{
	return this->setEntity("Date", date);
}

Date HttpHeader::getDate() const
{
	return Date::parseDate(DATE_FORMAT_GMT, getDateString());
}

void HttpHeader::setDate( const Date& date )
{
	return this->setDate(date.formatDate(DATE_FORMAT_GMT));
}

void HttpHeader::writeCrlfTo( OutputStream& output )
{
	output.writeBytes((byte*)CRLF, strlen(CRLF));
}



HttpRequest::HttpRequest( cstring url/*="/"*/ ) 
	:HttpHeader(),requestUrl(url),requestType(HTTP_GET)
{
}

HttpRequest::HttpRequest( dword len, cstring type ) 
	:HttpHeader(len, type),requestType(HTTP_GET),requestUrl("/")
{
}

HttpRequest::HttpRequest( const HashMap<String,String>& headers ) 
	:HttpHeader(headers),requestType(HTTP_GET),requestUrl("/")
{
}

HttpRequest::~HttpRequest()
{
}

const static cstring requestTypes[HttpRequest::REQUEST_TYPE_COUNT] = {
	"GET", "POST", "PUT", "DELETE", 
	"HEAD", "TRACE", "OPTIONS"
};
cstring HttpRequest::requestType2Str( RequestType val )
{
	if (0 <= val && val < REQUEST_TYPE_COUNT)
		return requestTypes[val];
	throw HttpException(String::format("invalid method: %d", val));
}

HttpRequest::RequestType HttpRequest::str2requestType( cstring val )
{
	checkNullPtr(val);
	RequestType type = REQUEST_TYPE_COUNT;
	for (int i=0; i<REQUEST_TYPE_COUNT; i++)
	{
		if (strcmp(requestTypes[i], val) == 0)
		{
			type = (RequestType)i;
			break;
		}
	}
	return type;
}

String HttpRequest::getParameter( const String& key, const String& default ) const
{
	return this->parameters.getDefault(key, default);
}

bool HttpRequest::setParameter( const String& key, const String& val )
{
	return this->parameters.put(key, val);
}

String HttpRequest::getAccept() const
{
	return this->geEntity("Accept", "");
}

void HttpRequest::setAccept( const String& val )
{
	return this->setEntity("Accept", val);
}

String HttpRequest::getAcceptCharset() const
{
	return this->geEntity("Accept-Charset", "");
}

void HttpRequest::setAcceptCharset( const String& val )
{
	return this->setEntity("Accept-Charset", val);
}

String HttpRequest::getAcceptEncoding() const
{
	return this->geEntity("Accept-Encoding", "");
}

void HttpRequest::setAcceptEncoding( const String& val )
{
	return this->setEntity("Accept-Encoding", val);
}

String HttpRequest::getUserAgent() const
{
	return this->geEntity("User-Agent", "");
}

void HttpRequest::setUserAgent( const String& val )
{
	return this->setEntity("User-Agent", val);
}

void HttpRequest::writeTo( OutputStream& output ) throw(Exception)
{
	//such as: "GET /index?paras HTTP/1.1"
	String url = getUrlWithParas();
	String line = String::format("%s %s %s/%s",
		requestType2Str(requestType), 
		url.c_str(), 
		name.c_str(), version.c_str());
	output.writeBytes((byte*)line.c_str(), line.length());
	writeCrlfTo(output);
	writeEntitiesTo(output);
	writeCrlfTo(output);
}

void HttpRequest::readFrom( InputStream& input ) throw(Exception)
{
	throw Exception("not implement");
}

void HttpRequest::writeCookiesTo( OutputStream& output ) throw(Exception)
{
	const String cookieTag = "Cookie: ";

	if (this->cookies.size() > 0)
	{
		StringBuilder cookies(this->cookies.size()*20);
		auto itor = this->cookies.iterator();
		while(itor->hasNext())
		{
			auto entry = itor->next();
			#define _c CodeUtil::urlEncodeComponent
			//Cookie: name=value; name2=value2
			cookies.append(_c(entry.key));
			cookies.append("=");
			cookies.append(_c(entry.value.value));
			cookies.append(";");
		}
		this->cookies.releaseIterator(itor);

		output.writeBytes((byte*)cookieTag.c_str(), cookieTag.length());		
		output.writeBytes((byte*)cookies.toString().c_str(), cookies.length());
		writeCrlfTo(output);
	}
}

void HttpRequest::readCookiesFrom( InputStream& input ) throw(Exception)
{
	throw Exception("not implement");
}

String HttpRequest::getParametersAsString() const
{
	auto& parameters = const_cast<HashMap<String,String>&>(this->parameters);

	StringBuilder sb(parameters.size()*20);
	auto itor = parameters.iterator();
	while(itor->hasNext())
	{
		auto entry = itor->next();
		//paras: name=value&name2=value2
		if(sb.length() > 0)
			sb.append("&");
		sb.append(CodeUtil::urlEncodeComponent(entry.key));
		sb.append("=");
		sb.append(CodeUtil::urlEncodeComponent(entry.value));
	}
	parameters.releaseIterator(itor);

	return sb.toString();
}

void HttpRequest::setParametersAsString(const String& paras)
{
	auto list = paras.splitWith("&");
	for(unsigned int i = 0; i < list.size(); i++)
	{
		String pair = list[i];
		auto kv = pair.splitWith("=");
		String k = CodeUtil::urlDecodeComponent(kv[0]);
		String v = kv.size() > 0 ? CodeUtil::urlDecodeComponent(kv[1]) : "";
		setParameter(k, v);
	}
}

String HttpRequest::getUrlWithParas() const
{
	String url = CodeUtil::urlEncode(requestUrl);
	String parameters = getParametersAsString();
	if(parameters.length() > 0)
		return url + "?" + parameters;
	else
		return url;
}

void HttpRequest::setUrlWithParas(const String& url)
{
	auto list = url.splitWith("?");
	setRequestUrl(CodeUtil::urlDecode(list[0]));
	if(list.size() > 1)
		setParametersAsString(list[1]);
}


HttpResponse::HttpResponse(const HashMap<String,String>& headers) 
	: HttpHeader(headers),status(Ok)
{
	String status = headers.getDefault(KEY_STATUS, "200 Ok");
	this->status = str2status(status);

	String tmp;
	this->entities.remove(KEY_STATUS, tmp);
	this->entities.remove(KEY_RESPONSE, tmp);
}

String HttpResponse::status2str(Status stat)
{
	String status;
	switch(stat)
	{
	case Ok:
		status="200 OK";
		break;
	case Created:
		status="201 Created";
		break;
	case Accepted:
		status="204 No Content";
		break;
	case ResetContent:
		status="205 Reset Content";
		break;
	case PartialContent:
		status="206 Partial Content";
		break;
	case MultiStatu:
		status="207 Multi-Statu";
		break;

	case FoundOther:
		status="302 Found";
		break;

	case BadRequest:
		status="400 Bad Request";
		break;
	case Forbidden:
		status="403 Forbidden";
		break;
	case NotFound:
		status="404 Not Found";
		break;
	case RequestTimeout:
		status="408 RequestTimeout";
		break;	
	case LengthRequired:
		status="411 Length Required";
		break;	
	case RequestEntityTooLarge:
		status="413 Request Entity Too Large";
		break;
	case RequestURITooLong:
		status="414 Request-URI Too Long";
		break;

	case InternalServerError:
		status="500 Internal Server Error";
		break;
	case NotImplemented:
		status="501 Not Implemented";
		break;
	case ServiceUnavailable:
		status="503 Service Unavailable";
		break;
	case HTTPVersionNotSupported:
		status="505 HTTP Version Not Supported";
		break;
	case BandwidthLimitExceeded:
		status="509 Bandwidth Limit Exceeded";
		break;

	case Default:
		status="0 Unknown Status";
		break;
	default:
		status=CodeUtil::int2Str(stat);
		break;
	}
	return status;
}

HttpResponse::Status HttpResponse::str2status(const String& status)
{
	auto list = status.splitWith(" ");//200 OK
	return (Status)CodeUtil::str2Int(list[0]);
	//return Status::Default;
}

String HttpResponse::getServer() const
{
	return this->geEntity("Server", "");
}

void HttpResponse::setServer( const String& val )
{
	return this->setEntity("Server", val);
}

void HttpResponse::writeTo( OutputStream& output ) throw(Exception)
{
	String line = String::format("%s/%s %s", //HTTP/1.0 200 OK 
		name.c_str(), version.c_str(), status2str(status).c_str());
	output.writeBytes((byte*)line.c_str(), line.length());
	writeCrlfTo(output);
	writeEntitiesTo(output);
	writeCrlfTo(output);
}

void HttpResponse::readFrom( InputStream& input ) throw(Exception)
{
	throw Exception("not implement");
}

void HttpResponse::writeCookiesTo( OutputStream& output ) throw(Exception)
{
	const String cookieTag = "Set-Cookie: ";
	auto itor = this->cookies.iterator();
	while(itor->hasNext())
	{
		auto entry = itor->next();
		output.writeBytes((byte*)cookieTag.c_str(), cookieTag.length());
		String cookieString = this->getCookieAsString(entry.key);
		output.writeBytes((byte*)cookieString.c_str(), cookieString.length());
		writeCrlfTo(output);
	}
	this->cookies.releaseIterator(itor);
}

void HttpResponse::readCookiesFrom( InputStream& input ) throw(Exception)
{
	throw Exception("not implement");
}

}//end of namespace brpc