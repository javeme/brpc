#pragma once
#include "Object.h"
#include "BString.h"
#include "HashMap.h"
#include "ByteBuffer.h"

namespace bluemei{

#define KEY_RESPONSE	 "Response"

#define KEY_CONTENT_TYPE "Content-Type"
#define KEY_CONTENT_LEN  "Content-Length"
#define KEY_PACKAGE_ID   "Package-Id"
#define KEY_CHARSET		 "Content-Charset"

/*
* 数据包
* @author 李章梅
 @create 2012/5/28
*/
class DataPackage : public Object
{
public:
	HashMap<String,String> headers;
	ByteBuffer body;
public:
	DataPackage(){}
	virtual ~DataPackage(){}

	DataPackage(DataPackage&& other){ *this = std::move(other); }
	DataPackage& operator=(DataPackage&& other){ 
		this->headers = std::move(other.headers);
		this->body = std::move(other.body);
		return *this; 
	}
	bool operator==(const DataPackage& other) const{ return this == &other; }
public:
	virtual String getId()const{ 
		return headers.getDefault(KEY_PACKAGE_ID, ""); 
	}
	static String genId(){
		unsigned int id = Util::random();
		return String::format("%u", id);
	}
/*private:
	DataPackage(const DataPackage& other){ *this = other; }
	DataPackage& operator=(const DataPackage& other){ return *this; }*/
};

}//end of namespace bluemei