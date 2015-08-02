#pragma once
#include "RpcSocket.h"
#include "RpcSerializerFactory.h"


namespace brpc{

#define HTTP_NAME		"HTTP"
#define HTTP_VERSION	"1.1"
#define HTTP_SERVER_NAME "bhttpd"

#define KEY_HTTP_VERSION	"version"
	
class HttpHeader : public Object
{
public:
	HttpHeader(){ init(); }
	HttpHeader(dword len, cstring type){ init(len, type); }
	HttpHeader(const HashMap<String,String>& headers);
	virtual ~HttpHeader(){}
public:
	class Cookie : public Object
	{
	public:
		String name;
		String value;
		Date expires;
		String domain;
		String path;
		bool secure;
	public:
		Cookie(const String& name="", const String& val="")
			:name(name), value(val),secure(false)
			,expires(Date::getCurrentTime()+DAY_MSECOND){}
		virtual ~Cookie(){}
	};
public:
	void init(dword len=0, cstring type="text/html",
		cstring version=HTTP_VERSION);

	virtual void writeTo(OutputStream& output) throw(Exception)=0;
	virtual void readFrom(InputStream& input) throw(Exception)=0;

	virtual void writeEntitiesTo(OutputStream& output) throw(Exception);
	virtual void readEntitiesFrom(InputStream& input) throw(Exception);

	virtual void writeCookiesTo(OutputStream& output) throw(Exception)=0;
	virtual void readCookiesFrom(InputStream& input) throw(Exception)=0;

	virtual void writeCrlfTo(OutputStream& output);

	virtual HashMap<String,String> getHeaders() const;
	virtual void update();//update such as contentType
public:
	static BrpcContentType str2contentType(cstring type);
	static cstring contentType2str(BrpcContentType type);
public:
	cstring getVersion() const { return version; }
	void setVersion(cstring val) { version = val; }

	dword getContentLength() const { return contentLen; }
	void setContentLength(dword val) { contentLen = val; }
	
	cstring getContentType() const;
	void setContentType(cstring val);
	String getContentTypeAndCharset() const;

	cstring getCharset() const { return charset; }
	void setCharset(cstring val) { charset = val; }
public:
	String geEntity(const String& key, const String& default)const;	
	void setEntity(const String& key, const String& val);
	unsigned int getEntrySize() const { return entities.size(); }
	bool contain(const String& key) const { return entities.contain(key); }

	HashMap<String,Cookie> getCookies() const { return cookies; }
	void setCookies(const HashMap<String,Cookie>& val) { cookies = val; }

	bool addCookie(const Cookie& val);
	bool addCookie(const String& name, const String& val);
	String getCookieAsString(const String& name) const;

	String getContentEncoding() const;
	void setContentEncoding(const String& val);

	String getConnection() const; // close/keepalive 
	dword getKeepAlive() const;
	void setKeepAlive(dword sec=20);

	String getDateString() const;
	void setDate(const String& date);
	Date getDate() const;
	void setDate(const Date& date);
protected:
	bool checkName() const;

protected:
	String name;//protocol http
	String version;//1.0 or 1.1 ...
	String contentType;
	String charset;
	dword contentLen;

	HashMap<String,String> entities;
	HashMap<String,Cookie> cookies;
};


class HttpRequest : public HttpHeader
{
public:
	enum RequestType{//method
		HTTP_GET=0, HTTP_POST, HTTP_PUT, HTTP_DELETE, 
		HTTP_HEAD, HTTP_TRACE, HTTP_OPTIONS,
		REQUEST_TYPE_COUNT
	};
	static cstring requestType2Str(RequestType val);
	static RequestType str2requestType(cstring val);
public:
	HttpRequest(cstring url="/");
	HttpRequest(dword len, cstring type);
	HttpRequest(const HashMap<String,String>& headers);
	virtual ~HttpRequest();
public:
	virtual void writeTo(OutputStream& output) throw(Exception);
	virtual void readFrom(InputStream& input) throw(Exception);

	virtual void writeCookiesTo(OutputStream& output) throw(Exception);
	virtual void readCookiesFrom(InputStream& input) throw(Exception);	
public:
	RequestType getRequestType() const { return requestType; }
	void setRequestType(RequestType val) { requestType = val; }

	String getRequestUrl() const { return requestUrl; }
	void setRequestUrl(const String& val) { requestUrl = val; }

	String getParameter(const String& key, const String& default) const;
	bool setParameter(const String& key, const String& val);
public:
	virtual String getParametersAsString() const;
	virtual void setParametersAsString(const String& paras);

	virtual String getUrlWithParas() const;
	virtual void setUrlWithParas(const String& url);
public:
	String getAccept() const;	//"text/html, image/*"
	void setAccept(const String& val);

	String getAcceptCharset() const;	//"iso8859-5"
	void setAcceptCharset(const String& val);

	String getAcceptEncoding() const;	//"gzip, compress"
	void setAcceptEncoding(const String& val);

	String getUserAgent() const;
	void setUserAgent(const String& val);
protected:
	RequestType requestType;
	String requestUrl;
	HashMap<String,String> parameters;
};


class HttpResponse : public HttpHeader
{
public:
	enum Status{
		/*200*/
		Ok=200,Created=201,
		Accepted=204,ResetContent=205,PartialContent=206,MultiStatu=207,
		/*302*/
		FoundOther=302,
		/*400*/
		BadRequest=400,Forbidden=403,NotFound=404,
		RequestTimeout=408,
		LengthRequired=411,RequestEntityTooLarge=413,RequestURITooLong=414,
		/*500*/
		InternalServerError=500,NotImplemented=501,
		ServiceUnavailable=503,HTTPVersionNotSupported=505,
		BandwidthLimitExceeded=509,
		Default=0
	};
	static String status2str(Status status);
	static Status str2status(const String& status);
public:
	HttpResponse(Status status=Ok):HttpHeader(),status(status){}
	HttpResponse(dword len, cstring type):HttpHeader(len, type),status(Ok){}
	HttpResponse(const HashMap<String,String>& headers);
	virtual ~HttpResponse(){}
public:
	virtual void writeTo(OutputStream& output) throw(Exception);
	virtual void readFrom(InputStream& input) throw(Exception);

	virtual void writeCookiesTo(OutputStream& output) throw(Exception);
	virtual void readCookiesFrom(InputStream& input) throw(Exception);

public:
	Status getStatus() const { return status; }
	void setStatus(Status val) { status = val; }

	String getDescription() const { return description; }
	void setDescription(const String& val) { description = val; }

	String getServer() const;
	void setServer(const String& val=HTTP_SERVER_NAME);
protected:
	Status status;
	String description;
};

}//end of namespace bluemei