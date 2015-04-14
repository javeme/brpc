#pragma once
#include "RpcSerializeable.h"

namespace bluemei{

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

	bool registerSerializer(cstring name, const Class& serializer);
	bool unregisterSerializer(cstring name);
public:
	static BrpcContentType str2contentType(cstring type);
	static cstring contentType2str(BrpcContentType type);
private: 
	RpcSerializerFactory();
	virtual ~RpcSerializerFactory();
private:
	Map<String, const Class&> str2SerializerMap;
};


struct SerializerReg
{
	SerializerReg(cstring name, const Class& cls) : name(name)
	{
		RpcSerializerFactory::instance().registerSerializer(name, cls);
	}
	virtual ~SerializerReg()
	{
		RpcSerializerFactory::instance().unregisterSerializer(name);
	}
	String name;
};
#define REG_SERIALIZER(name, cls) \
static SerializerReg cls##SerializerReg(name, *cls::thisClass());

}//end of namespace bluemei