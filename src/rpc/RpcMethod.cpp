#include "RpcMethod.h"

namespace brpc{

RpcMethod::RpcMethod()
{
	this->autoDelArgs = false;
	reset();
}

RpcMethod::RpcMethod( cstring name, ObjectList* args, bool waitResult/*=true*/, cstring authToken/*=""*/ )
{
	this->autoDelArgs = false;
	reset();

	this->status = STATUS_REQUEST;
	this->authToken = authToken;
	this->methodName = name;
	this->args = args;
	this->waitResult = waitResult;
}

RpcMethod::RpcMethod( cstring obj, cstring name, ObjectList* args, bool waitResult/*=true*/, cstring authToken/*=""*/ )
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

RpcMethod::RpcMethod( Object* returnValue, cstring name, cstring obj/*=""*/, int status/*=STATUS_RESPONSE_OK*/, cstring authToken/*=""*/ )
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

RpcMethod::~RpcMethod()
{
	if (isAutoDelArgs())
	{
		delete args;
		//delete returnValue;
	}
}

void RpcMethod::reset()
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

String RpcMethod::getFullName() const
{
	String methodName = this->methodName;
	if(owner.length() > 0)
		methodName = owner + "." + methodName;
	return methodName;
}

String RpcMethod::getMethodNameWithVersion() const
{
	String methodName = this->methodName;
	if(version != currentVersion())
		methodName = methodName + version;
	return methodName;
}

cstring RpcMethod::currentVersion() const
{
	return CURRENT_RPC_METHOD_VERSION;
}

bool RpcMethod::isRequest() const
{
	if (status == RpcMethod::STATUS_REQUEST
		||status == RpcMethod::STATUS_REQUEST_LOGIN
		||status == RpcMethod::STATUS_REQUEST_LOGOUT)
		return true;
	return false;
}

bool RpcMethod::isEvent() const
{
	if (status == RpcMethod::STATUS_RESPONSE_EVENT)
		return true;
	return false;
}

String RpcMethod::getResultString() const
{
	String* str = dynamic_cast<String*>(returnValue);
	cstring msg = (str == null) ? "" : str->c_str();
	return msg;
}

void RpcMethod::releaseResult()
{
	delete returnValue;
	returnValue = null;
}

String RpcMethod::getResultStringAndRelease()
{
	String msg = getResultString();
	releaseResult();
	return msg;
}

}