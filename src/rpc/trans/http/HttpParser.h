#pragma once
#include "src/rpc/trans/http/HttpHeader.h"


namespace brpc{

#define HTTP_LINE_END "\n"

class HttpParser : public Object
{
public:
	HttpParser();
	virtual ~HttpParser();
public:
	ScopePointer<HttpHeader>  parse(const String& text);
	ScopePointer<HttpHeader>  parse(const LinkedList<String>& lines);
public:
	static HashMap<String,String> paramSplit(const String& str);
	static HashMap<String,String> parseCookies(const String& str);
	static String parseContentType(const String& type);//".html" -> "text/html"
	static String decodeUrl(const String& url);
};

}//end of namespace brpc
