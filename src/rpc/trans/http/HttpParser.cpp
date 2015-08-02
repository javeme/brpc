#pragma once
#include "stdafx.h"
#include "HttpParser.h"
#include "HttpException.h"

namespace brpc{

HttpParser::HttpParser()
{
}

HttpParser::~HttpParser()
{
}

SmartPtr<HttpHeader> HttpParser::parse(const String& text)
{
	//@Todo: parse from string
	;
	return null;
}

SmartPtr<HttpHeader> HttpParser::parse(const LinkedList<String>& lines)
{
	SmartPtr<HttpHeader> header = null;

	String line = lines.get(0);	
	//Response
	if (line.startWith(HTTP_NAME))//HTTP/1.0 200 OK 
	{
		auto list = line.splitWith(" ");
		if(list.size() < 3)
			throw HttpParseException("Invalid http response: "+line);
		SmartPtr<HttpResponse> response = new HttpResponse();
		header = response; 
		response->setVersion(list[0].splitWith("/")[1]);
		response->setStatus(HttpResponse::str2status(list[1]));
		response->setDescription(line.substring(line.find(list[2],list[0].length())));
	}
	//Request
	else//GET /index HTTP/1.1
	{
		auto list = line.splitWith(" ");
		if(list.size() != 3)
			throw HttpParseException("Invalid http request");

		SmartPtr<HttpRequest> request= new HttpRequest();
		header = request;
		request->setRequestType(HttpRequest::str2requestType(list[0]));
		request->setUrlWithParas(list[1]);
		request->setVersion(list[2].splitWith("/")[1]);
	}
	
	//Entities
	auto itor = const_cast<LinkedList<String>&>(lines).iterator();
	while(itor->hasNext())
	{
		line = itor->next();
		int offset = line.find(":");
		if(offset<=0)
			continue;
		String key = line.getLeftSub(offset);
		String val = line.getRightSub(line.length() - offset -1);
		header->setEntity(key.trim(), val.trim());
	}
	const_cast<LinkedList<String>&>(lines).releaseIterator(itor);

	header->update();
	return header;
}

HashMap<String,String> HttpParser::paramSplit(const String& str)
{
	HashMap<String,String> paras;

	//½âÎö²ÎÊý
	auto list = str.splitWith("&");
	for(unsigned int i = 0; i < list.size(); i++)
	{
		String pair = list[i];
		auto kv = pair.splitWith("=");
		String k = CodeUtil::urlDecodeComponent(kv[0]);
		String v = kv.size() > 0 ? CodeUtil::urlDecodeComponent(kv[1]) : "";
		paras.put(k, v);
	}

	return paras;
}

HashMap<String,String> HttpParser::parseCookies(const String& str)
{
	HashMap<String,String> cookies;

	String key,value;
	int positionStart=0, positionEnd=0,position1=0,position2=0;
	bool end = false;
	while(!end)
	{
		position1=str.find("=",position1);
		if(position1>=0)
			key=str.substring(position2,position1-position2);
		else
			break ;
		position2=str.find(";",position1);
		if(position2 <= position1)
		{
			end = true;			
		}

		value=str.substring(position1+1,position2-position1-1);
		value=CodeUtil::urlDecodeComponent(value.c_str());

		cookies.put(key,value);
		position1=position2=position2+1;
	}
	return cookies;
}

String HttpParser::parseContentType(const String& type)
{
	String contentType;

	if(type.compare("html")==0||type.compare("htm")==0
		||type.compare("HTML")==0||type.compare("HTM")==0)
		contentType="text/html";
	else if(type.compare("text")==0||type.compare("TEXT")==0)
		contentType="text/plain";
	else if(type.compare("gif")==0||type.compare("GIF")==0) 
		contentType="image/gif";
	else if(type.compare("jpeg")==0||type.compare("jpg")==0
		||type.compare("JPEG")==0||type.compare("JPG")==0) 
		contentType="image/jpeg";
	else if(type.compare("mp3")==0||type.compare("MP3")==0) 
		contentType="audio/x-mpeg";
	else if(type.compare("wma")==0||type.compare("WMA")==0) 
		contentType="audio/x-ms-wma";
	else if(type.compare("mid")==0||type.compare("MID")==0)
		contentType="audio/midi";
	else if(type.compare("mp4")==0||type.compare("MP4")==0)
		contentType="video/mpeg";
	else if(type.compare("gz")==0||type.compare("GZ")==0)
		contentType="application/x-gzip";
	else
	{		
		contentType="application/"+type;
	}
	return contentType;
}

String HttpParser::decodeUrl(const String& url)
{
	return CodeUtil::urlDecode(url);
}

}//end of namespace bluemei