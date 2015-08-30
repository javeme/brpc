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

void RpcMethod::writeTo(ObjectMap& methodObjMap) const
{
	methodObjMap.putValue("version",this->version);
	methodObjMap.putValue("status",this->status);
	methodObjMap.putValue("authToken",this->authToken);
	methodObjMap.putValue("owner",this->owner);
	methodObjMap.putValue("method",this->methodName);
	methodObjMap.putValue("needReturn",this->waitResult);
	methodObjMap.put("args", this->args);
	methodObjMap.put("returnValue", this->returnValue);
}

void RpcMethod::readFrom(const ObjectMap& methodObjMap)
{
	methodObjMap.getValue("version",this->version);
	methodObjMap.getValue("status",this->status);
	methodObjMap.getValue("authToken",this->authToken);
	methodObjMap.getValue("owner",this->owner);
	methodObjMap.getValue("method",this->methodName);
	methodObjMap.getValue("needReturn",this->waitResult);
	this->args = methodObjMap.get("args");
	this->returnValue = methodObjMap.get("returnValue");
}

}