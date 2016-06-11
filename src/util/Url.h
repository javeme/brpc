#pragma once
#include "blib.h"

namespace brpc{

class Url : public Object
{
public:
	Url(const String& url="");
	virtual ~Url() {}
	
public:
	virtual String toString()const { return url; }

	String getProtocol(const String& deft) const {
		if(protocol.empty())
			return deft;
		return protocol; 
	}
	String getHost(const String& deft="127.0.0.1") const {
		if(ip.empty())
			return deft;
		return ip; 
	}
	String getPort(const String& deft) const {
		if(port.empty())
			return deft;
		return port; 
	}
	String getPath(const String& deft="") const {
		if(path.empty())
			return deft;
		return path; 
	}
	String getParameter(const String& key, const String& deft="") const {
		String* val = params.get(key);
		if(val == null)
			return deft;
		return *val; 
	}
protected:
	virtual void parseUrl();
protected:
	String url;

	String protocol;
	String ip;
	String port;
	String path;
	HashMap<String, String> params;
};

}//end of namespace brpc