#pragma once
#include "Url.h"

namespace brpc{

///////////////////////////////////////////////////////////////////////////
//class Url
Url::Url( const String& url/*=""*/ ) : url(url)
{
	if(!url.empty()) {
		try {
			parseUrl();
		} catch(Exception& e) {
			String msg = String::format(
				"Faild to parse URL '%s' due to: %s",
				url.c_str(), e.toString().c_str());
			throw InvalidArgException(msg);
		}
	}
}

void Url::parseUrl()
{
	String addr;
	if(url.contain("://")) { // like http://127.0.0.1:8080
		auto list = url.splitWith("://");
		protocol = list[0];
		addr = list[1];
	}
	else {
		addr = url;
	}

	if(addr.contain("/")) { // like 127.0.0.1:8080/test/index.html
		auto list = addr.splitWith("/", 2);
		addr = list[0];
		path = list[1];
	}
	else if(addr.contain("?")) {
		auto list = addr.splitWith("?", 2);
		addr = list[0];
		path = "?" + list[1];
	}

	if(addr.contain(":")) { // like 127.0.0.1:8080
		auto list = addr.splitWith(":");
		ip = list[0];
		port = list[1];
	}
	else { // like 127.0.0.1
		ip = addr;
	}

	if (path.contain("?")) {
		auto list = path.splitWith("?", 2);
		path = list[0];
		// parameters
		list = list[1].splitWith("&");
		for(unsigned int i = 0; i < list.size(); i++)
		{
			String pair = list[i];
			auto kv = pair.splitWith("=");
			String k = CodeUtil::urlDecodeComponent(kv[0]);
			String v = kv.size() > 1 ? CodeUtil::urlDecodeComponent(kv[1]) : "";
			params.put(k, v);
		}
	}
}

}//end of namespace brpc