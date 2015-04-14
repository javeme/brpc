#pragma once
#include "stdafx.h"
#include "Object.h"
#include "ObjectList.h"

namespace bluemei{

const static cstring CURRENT_RPC_METHOD_VERSION = "1.0";

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
	RpcMethod()
	{		
		this->autoDelArgs = false;
		reset();
	}
	RpcMethod(cstring name, ObjectList* args, 
		bool waitResult=true, cstring authToken="")
	{
		this->autoDelArgs = false;
		reset();

		this->status = STATUS_REQUEST;
		this->authToken = authToken;
		this->methodName = name;
		this->args = args;
		this->waitResult = waitResult;
	}
	RpcMethod(cstring obj, cstring name, ObjectList* args, 
		bool waitResult=true, cstring authToken="")
	{
		this->autoDelArgs = false;
		reset();

		this->status = STATUS_REQUEST;
		this->authToken = authToken;
		this->owner = obj;
		this->methodName = name;
		this->args = args;
		this->waitResult = waitResult;
	}
	RpcMethod(Object* returnValue, cstring name, cstring obj="", 
		int status=STATUS_RESPONSE_OK, cstring authToken="")
	{
		this->autoDelArgs = false;
		reset();

		this->status = status;
		this->authToken = authToken;
		this->owner = obj;
		this->methodName = name;
		this->returnValue = returnValue;
		this->waitResult = false;
	}
	virtual ~RpcMethod()
	{
		if (isAutoDelArgs())
		{
			delete args;
			//delete returnValue;
		}
	}

	virtual void reset()
	{
		this->version = currentVersion();
		this->status = STATUS_REQUEST;
		this->authToken = "";
		this->owner = "";
		this->methodName = "";	

		if (isAutoDelArgs())
		{
			delete args;
			//delete returnValue;
		}
		this->args = null;
		this->returnValue = null;
		this->waitResult = false;
		//this->autoDelArgs = false;
	}
	virtual String getFullName() const
	{
		String methodName = this->methodName;
		if(owner.length() > 0)
			methodName = owner + "." + methodName;
		return methodName;
	}
	virtual String getMethodNameWithVersion() const
	{
		String methodName = this->methodName;
		if(version != currentVersion())
			methodName = methodName + version;
		return methodName;
	}

	virtual cstring currentVersion() const
	{
		return CURRENT_RPC_METHOD_VERSION;
	}

	bool isRequest() const
	{
		if (status == RpcMethod::STATUS_REQUEST
			||status == RpcMethod::STATUS_REQUEST_LOGIN
			||status == RpcMethod::STATUS_REQUEST_LOGOUT)
			return true;
		return false;
	}

	bool isEvent() const
	{
		if (status == RpcMethod::STATUS_RESPONSE_EVENT)
			return true;
		return false;
	}

	virtual String getResultString() const
	{
		String* str = dynamic_cast<String*>(returnValue);
		cstring msg = (str == null) ? "" : str->c_str();
		return msg;
	}

	virtual void releaseResult()
	{
		delete returnValue;
		returnValue = null;
	}
	virtual String getResultStringAndRelease()
	{
		String msg = getResultString();
		releaseResult();
		return msg;
	}

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