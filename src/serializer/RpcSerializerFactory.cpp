#pragma once
#include "stdafx.h"
#include "RpcSerializerFactory.h"

namespace brpc{


RpcSerializerFactory::RpcSerializerFactory()
{
	;
}

RpcSerializerFactory::~RpcSerializerFactory()
{
	;
}

RpcSerializeable* RpcSerializerFactory::getSerializer(cstring name)
{
	auto itor = str2SerializerMap.find(name);
	if (itor == str2SerializerMap.end())
		return null;
	/*const Class& cls = itor->second;
	Object* obj = cls.createObject();
	RpcSerializeable* serializer = dynamic_cast<RpcSerializeable*>(obj);
	if(serializer == null)
		delete obj;
	return serializer;*/
	return itor->second;
}

RpcSerializeable* RpcSerializerFactory::getSerializer(BrpcContentType type)
{
	return getSerializer(contentType2str(type));
}

bool RpcSerializerFactory::registerSerializer(cstring name, RpcSerializeable* serializer)
{
	return str2SerializerMap.insert(std::make_pair(name, serializer)).second;
}

bool RpcSerializerFactory::unregisterSerializer(cstring name)
{
	return str2SerializerMap.erase(name) > 0;
}

RpcSerializerFactory& RpcSerializerFactory::instance()
{
	static RpcSerializerFactory factory;
	return factory;
}

struct ContentTypeMapIniter
{
	Map<BrpcContentType, String> contentType2strMap;
	Map<String, BrpcContentType> str2contentTypeMap;
	ContentTypeMapIniter()
	{
		#define ct2s(k,v) contentType2strMap[k]=v
		ct2s(BRPC_TYPE_BIN, "application/brpc.bin");//application/octet-stream
		ct2s(BRPC_TYPE_JSON, "text/json");
		ct2s(BRPC_TYPE_XML, "text/xml");
		

		for(auto i=contentType2strMap.begin();i!=contentType2strMap.end();i++)
		{
			str2contentTypeMap[i->second]=i->first;
		}
	}
	BrpcContentType str2contentType(cstring type) const
	{
		BrpcContentType contentType = BRPC_TYPE_UNKNOWN;
		auto itor = str2contentTypeMap.find(type);
		if(itor != str2contentTypeMap.end())
			contentType = itor->second;
		return contentType;
	}
	cstring contentType2str(BrpcContentType type) const
	{
		auto itor = contentType2strMap.find(type);
		if(itor != contentType2strMap.end())
			return itor->second;
		return "";
	}
};
static ContentTypeMapIniter contentTypeMapIniter;


BrpcContentType RpcSerializerFactory::str2contentType(cstring type)
{
	return contentTypeMapIniter.str2contentType(type);
}

cstring RpcSerializerFactory::contentType2str(BrpcContentType type)
{
	return contentTypeMapIniter.contentType2str(type);
}


}//end of namespace brpc