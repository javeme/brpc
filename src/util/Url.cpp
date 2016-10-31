#pragma once
#include "src/util/Url.h"
#include "blib/CodeUtil.h"

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
		this->protocol = list[0];
		addr = list[1];
	}
	else {
		addr = url;
	}

	if(addr.contain("@")) { // like user:password@127.0.0.1
		auto list = addr.splitWith("@", 2);
		addr = list[1];
		list = list[0].splitWith(":", 2);
		this->username = list[0];
		if(list.size() > 1)
			this->password = list[1];
	}

	if(addr.contain("/")) { // like 127.0.0.1:8080/test/index.html
		auto list = addr.splitWith("/", 2);
		addr = list[0];
		this->path = list[1];
	}
	else if(addr.contain("?")) {
		auto list = addr.splitWith("?", 2);
		addr = list[0];
		this->path = "?" + list[1];
	}

	if(addr.contain(":")) { // like 127.0.0.1:8080
		auto list = addr.splitWith(":");
		this->ip = list[0];
		this->port = list[1];
	}
	else { // like 127.0.0.1
		this->ip = addr;
	}

	if (path.contain("?")) {
		auto list = path.splitWith("?", 2);
		this->path = list[0];
		// parameters
		list = list[1].splitWith("&");
		for(unsigned int i = 0; i < list.size(); i++)
		{
			String pair = list[i];
			auto kv = pair.splitWith("=");
			String k = CodeUtil::urlDecodeComponent(kv[0]);
			String v = kv.size() > 1 ? CodeUtil::urlDecodeComponent(kv[1]) : "";
			this->params.put(k, v);
		}
	}
}

}//end of namespace brpc