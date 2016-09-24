#pragma once
#include "stdafx.h"
#include "RpcApi.h"
#include "ErrorHandler.h"
#include "RpcClient.h"
#include "DefaultAuthChecker.h"
#include "JsonSerializer.h"
using namespace brpc;

class MySubRpcApi : public RpcApi
{
public:
	MySubRpcApi(RpcApi* parent) : RpcApi("sub", parent){}
public:
	int print(cstring str)
	{
		ObjectList args;
		args.addValue(str);
		return call<int>("print", args);
	}
};

class CallbackRpcService : public RpcService
{
public:
	CallbackRpcService(cstring serviceName) : RpcService(serviceName)
	{
		as(CallbackRpcService::onclick);
	}
	void onclick(cstring event, int count)
	{
		printf(">>>>on event click(%s): %d\n", event, count);
	}
};

class MyRpcApi : public RpcApi
{
public:
	MyRpcApi(cstring url, cstring name, cstring password) :
		checker("", ""),
		dispatcher("callback"),
		client(url, &dispatcher, &checker, "text/json", 1000*10),
		//RpcApi(&client, loginArgs)
		//_init1(loginArgs.addValue(name)),
		//_init2(loginArgs.addValue(password)),
		//RpcApi(client=new P2pRpcClient(url, &dispatcher, &serializer, &checker), loginArgs)
		RpcApi("nova")
	{
		ObjectList loginArgs;
		loginArgs.addValue(name);
		loginArgs.addValue(password);
		init(&client, loginArgs);

		subRpcApi = new MySubRpcApi(this);
	}
	virtual ~MyRpcApi()
	{
		delete subRpcApi;
		try{
			logout();
		}catch (Exception& e) {
			ErrorHandler::handle(e);
		}
	}
public:
	int print(cstring str)
	{
		ObjectList args;
		args.addValue(str);
		return call<int>("print", args);
	}
	int print(TestObject* obj)
	{
		ObjectList args;
		args.addValue(objectToMap(obj));
		return call<int>("print", args);
	}
	double sum(int a, double b)
	{
		ObjectList args;
		args.addValue(a);
		args.addValue(b);
		return call<double>("sum", args);
	}
	bool playMusic(cstring name)
	{
		ObjectList args;
		args.addValue(name);
		return call<bool>("playMusic", args);
	}
	String execute(cstring cmd)
	{
		ObjectList args;
		args.addValue(cmd);
		return call<String>("execute", args);
	}
private:
	CallbackRpcService dispatcher;
	DefaultAuthChecker checker;
	RpcClient client;
	//ObjectList loginArgs;
	//unsigned int _init1, _init2;

	MySubRpcApi* subRpcApi;
};