#pragma once
#include "stdafx.h"
#include "Object.h"
#include "ObjectList.h"

namespace bluemei{

const static cstring CURRENT_RPC_METHOD_VERSION = "1.0";

const static unsigned int BRPC_MAX_BODY_LEN = 1024 * 1024 * 4;//4M

/*
* RPC 方法封装
* @author 李章梅
* @create 2014/9/21
*/
class RpcMethod : public Object
{
public:
	enum{ 
		STATUS_REQUEST		 = 10,
		STATUS_REQUEST_LOGIN = 11,
		STATUS_REQUEST_LOGOUT= 12,

		STATUS_RESPONSE_OK			= 20,
		STATUS_RESPONSE_EVENT		= 26,
		STATUS_RESPONSE_PENDING		= 27,
		STATUS_RESPONSE_EXCEPTION	= 28,

		STATUS_RESPONSE_ERROR			= 30,
		STATUS_RESPONSE_INVALID_METHOD	= 31,
		STATUS_RESPONSE_INVALID_ARGS	= 32,
		STATUS_RESPONSE_DENIED			= 33,	
	};
public:
	RpcMethod();
	RpcMethod(cstring name, ObjectList* args, 
		bool waitResult=true, cstring authToken="");
	RpcMethod(cstring obj, cstring name, ObjectList* args, 
		bool waitResult=true, cstring authToken="");
	RpcMethod(Object* returnValue, cstring name, cstring obj="", 
		int status=STATUS_RESPONSE_OK, cstring authToken="");
	virtual ~RpcMethod();

public:
	virtual void reset();
	virtual String getFullName() const;
	virtual String getMethodNameWithVersion() const;
	virtual cstring currentVersion() const;

	bool isRequest() const;
	bool isEvent() const;

	virtual String getResultString() const;
	virtual void releaseResult();
	virtual String getResultStringAndRelease();

public:
	String version;//such as "1.0"
	int status;//request or response status
	String authToken;

	String owner;//this object
	String methodName;//name with namaspace
	Object* args;
	Object* returnValue;

	bool waitResult;

public:
	bool isAutoDelArgs() const { return autoDelArgs; }
	void setAutoDelArgs(bool val) { autoDelArgs = val; }
	
private:
	bool autoDelArgs;
};


}//end of namespace bluemei