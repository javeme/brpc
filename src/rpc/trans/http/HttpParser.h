#pragma once
#include "HttpHeader.h"

namespace bluemei{

#define HTTP_LINE_END "\n"

class HttpParser : public Object
{
public:
	HttpParser();
	virtual ~HttpParser();
public:	
	HttpHeader* parse(const String& text);
	HttpHeader* parse(const LinkedList<String>& lines);
public:	
	static HashMap<String,String> paramSplit(const String& str);
	static HashMap<String,String> parseCookies(const String& str);
	static String parseContentType(const String& type);//".html" -> "text/html"
	static String decodeUrl(const String& url);
};

}//end of namespace bluemei