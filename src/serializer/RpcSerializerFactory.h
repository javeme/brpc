#pragma once
#include "RpcSerializeable.h"

namespace brpc{

enum BrpcContentType
{
	BRPC_TYPE_UNKNOWN=0,
	BRPC_TYPE_BIN=1,
	BRPC_TYPE_JSON=2,
	BRPC_TYPE_XML=3,
};

class RpcSerializerFactory : public Object
{
public:
	static RpcSerializerFactory& instance();
public:
	RpcSerializeable* getSerializer(cstring name);
	RpcSerializeable* getSerializer(BrpcContentType type);

	bool registerSerializer(cstring name, RpcSerializeable* serializer);
	bool unregisterSerializer(cstring name);
public:
	static BrpcContentType str2contentType(cstring type);
	static cstring contentType2str(BrpcContentType type);
private: 
	RpcSerializerFactory();
	virtual ~RpcSerializerFactory();
private:
	Map<String, RpcSerializeable*> str2SerializerMap;
};


template <typename Cls>
struct SerializerReg
{
	SerializerReg(cstring name) : name(name)
	{
		RpcSerializeable* serializer = &this->serializer;
		RpcSerializerFactory::instance().registerSerializer(name, serializer);
	}
	virtual ~SerializerReg()
	{
		RpcSerializerFactory::instance().unregisterSerializer(name);
	}
	String name;
	Cls serializer;
};

#define REG_SERIALIZER(name, cls) \
	static SerializerReg<cls> ANONYMOUS(__s_regFor##cls)(name);

}//end of namespace brpc