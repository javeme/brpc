#pragma once
#include "blib/ErrorHandler.h"
#include "src/rpc/RpcConnection.h"
#include "src/serializer/RpcSerializerFactory.h"


namespace brpc{

RpcConnection::RpcConnection(RpcService* dispatcher,
	AuthChecker* authChecker, cstring serializerType)
	: RpcInvoker(),
	dispatcher(dispatcher),
	authChecker(authChecker),
	serializerType(serializerType)
{
	checkNullPtr(this->dispatcher);
	checkNullPtr(this->authChecker);

	this->serializer = getSerializer(serializerType);
	checkNullPtr(this->serializer);
}

RpcConnection::~RpcConnection()
{
	;
}


Object* RpcConnection::call(cstring name, const ObjectList& args)
{
	return call("", name, args);
}

Object* RpcConnection::onCall(cstring name, const ObjectList& args)
{
	return onCall("", name, args);
}

void RpcConnection::cast(cstring name, const ObjectList& args)
{
	return cast("", name, args);
}


Object* RpcConnection::call(cstring obj, cstring name, const ObjectList& args)
{
	RpcMethod req(obj, name, const_cast<ObjectList*>(&args), true, authToken);
	RpcMethod methodResult;
	sendCall4Result(req, methodResult);
	if (methodResult.status == RpcMethod::STATUS_RESPONSE_OK)
		return methodResult.returnValue;
	else{
		String msg = methodResult.getResultStringAndRelease();
		throw RpcCallException(methodResult.status, msg);
	}
}

Object* RpcConnection::onCall(cstring obj, cstring name, const ObjectList& args)
{
	checkNullPtr(dispatcher);
	RpcContext ctx(*this);
	return dispatcher->call(&ctx, obj, name, args);
}

void RpcConnection::castMethod(const Headers& headers, const RpcMethod& method)
{
	checkConnected();

	DataPackage package;
	String contentType = headers.getDefault(KEY_CONTENT_TYPE, "");
	String charset = headers.getDefault(KEY_CHARSET, "");
	RpcSerializeable* serializer = getSerializer(contentType);
	checkNullPtr(serializer);
	package.headers = headers;//headers empty?????
	OutputStream& output = package.body;
	(void)serializer->write(output, method, charset);
	onSend(package);
}

void RpcConnection::cast(cstring obj, cstring name, const ObjectList& args)
{
	checkConnected();

	Headers headers;
	headers.put(KEY_CONTENT_TYPE, serializerType);
	headers.put(KEY_CHARSET, "GBK");
	RpcMethod method(obj, name, const_cast<ObjectList*>(&args),
		false, authToken);
	castMethod(headers, method);
}

void RpcConnection::sendEvent(const Headers& headers,
	cstring method, const ObjectList& args)
{
	RpcMethod req(method, const_cast<ObjectList*>(&args), false);
	req.status = RpcMethod::STATUS_RESPONSE_EVENT;
	try{
		castMethod(headers, req);
	}catch (Exception& e){
		(void)onReturnCallException(e);
	}
}


bool RpcConnection::doCallResponse(const Headers& headers,
	const RpcMethod& method, Object* result, int status)
{
	Headers rp;//reponse headers
	rp.put(KEY_RESPONSE, "true");
	if(headers.contain(KEY_CONTENT_TYPE)){
		String contentType = *headers.get(KEY_CONTENT_TYPE);
		rp.put(KEY_CONTENT_TYPE, contentType);
	}
	if(headers.contain(KEY_CHARSET)){
		String charset = headers.getDefault(KEY_CHARSET, "");
		rp.put(KEY_CHARSET, charset);
	}
	if(headers.contain(KEY_PACKAGE_ID))
		rp.put(KEY_PACKAGE_ID, *headers.get(KEY_PACKAGE_ID));

	bool success = false;
	try{
		castMethod(rp, RpcMethod(result, method.methodName, method.owner,
								 status, method.authToken));
		success = true;
	}catch (Exception& e){
		success = onReturnCallException(e);
	}
	return success;
}

bool RpcConnection::doCall(const Headers& headers, RpcMethod& method)
{
	bool success = false;
	if (!method.waitResult){
		success = doCallResponse(headers, method, null,
			RpcMethod::STATUS_RESPONSE_PENDING);
	}

	ObjectList* args = dynamic_cast<ObjectList*>(method.args);
	String methodName = method.getMethodNameWithVersion();

	int status = 0;
	ScopePointer<Object> result = null;

	try{
		checkAuth(method);
		if(args == null)
			throw ArgNotMatchedException("invalid args(args can't be null)");
		result = onCall(method.owner, methodName, *args);
		status = RpcMethod::STATUS_RESPONSE_OK;
	}catch (AuthException& e){
		status = RpcMethod::STATUS_RESPONSE_DENIED;
		result = new String(e.toString());
	}catch (NotFoundException& e){
		status = RpcMethod::STATUS_RESPONSE_INVALID_METHOD;
		result = new String(e.toString());
	}catch (ArgNotMatchedException& e){
		status = RpcMethod::STATUS_RESPONSE_INVALID_ARGS;
		result = new String(e.toString());
	}catch (LoginException& e){//login
		method.authToken = e.getAuthToken();//set auth-token
		status = RpcMethod::STATUS_RESPONSE_OK;
		result = new String(e.what());
	}catch (LogoutException& e){//logout
		status = RpcMethod::STATUS_RESPONSE_OK;
		result = new String(e.what());
	}catch (Exception& e){
		status = RpcMethod::STATUS_RESPONSE_EXCEPTION;
		result = new String(e.toString());
	}catch (...){
		status = RpcMethod::STATUS_RESPONSE_ERROR;
		result = new String("system error");
	}

	if (method.waitResult){
		success = doCallResponse(headers, method, result, status);
	}

	return success;
}

//received notify-event
bool RpcConnection::doEvent(const Headers& headers, RpcMethod& method)
{
	bool success = false;

	ObjectList* args = dynamic_cast<ObjectList*>(method.args);
	String methodName = method.getMethodNameWithVersion();
	try{
		if(args == null)
			throw ArgNotMatchedException("invalid args(args can't be null)");
		delete onCall(method.owner, methodName, *args);
		success = true;
	}catch (Exception& e){
		success = false;
		ErrorHandler::handle(e);
	}
	return success;
}

bool RpcConnection::invoke(const Headers& headers, const InputStream& input)
{
	RpcMethod method;
	try{
		String contentType = headers.getDefault(KEY_CONTENT_TYPE, "");
		String charset = headers.getDefault(KEY_CHARSET, "");
		RpcSerializeable* serializer = getSerializer(contentType);
		checkNullPtr(serializer);
		if (serializer->read(method, input, charset) > 0){
			if (method.isRequest())
				return this->doCall(headers, method);
			else if (method.isEvent())
				return this->doEvent(headers, method);
			else//ignore
				return true;
		}
	}catch (SerializeException& e)
	{
		String str = e.toString();
		//@TODO(lzm): does it need to response if it's an event?
		bool r = doCallResponse(headers, method, &str, RpcMethod::STATUS_RESPONSE_ERROR);
		bool s = onSerializeException(e);
		return (r && s);
	}
	return false;
}

void RpcConnection::sendCall4Result(const RpcMethod& method, RpcMethod& methodResult)
{
	checkConnected();

	DataPackage package;
	package.headers.put(KEY_CONTENT_TYPE, serializerType);
	package.headers.put(KEY_CHARSET, "GBK");
	package.headers.put(KEY_PACKAGE_ID, DataPackage::genId());

	OutputStream& output = package.body;
	checkNullPtr(serializer);
	(void)serializer->write(output, method);
	DataPackage result = onSendSynch(package);

	InputStream& input = result.body;
	String contentType = result.headers.getDefault(KEY_CONTENT_TYPE, "");
	RpcSerializeable* inputSerializer = getSerializer(contentType);
	(void)inputSerializer->read(methodResult, input);
}

void RpcConnection::checkAuth(const RpcMethod& method)
{
	checkNullPtr(authChecker);
	if(authChecker->checkAuth(method.authToken, method.getFullName())){
		String methodName = method.getMethodNameWithVersion();
		//methodName == "logout"
		if (method.status == RpcMethod::STATUS_REQUEST_LOGOUT)
		{
			try{
				bool success = authChecker->logout(method.authToken);
				cstring msg = success ? "logout success" : "logout failed";
				throw LogoutException(msg);
			}catch (LogoutException&){
				throw;
			}catch (Exception& e){
				throw AuthException(e.toString());
			}
		}
		return;
	}
	else {
		String methodName = method.getMethodNameWithVersion();
		//methodName == "login"
		if (method.status == RpcMethod::STATUS_REQUEST_LOGIN)
		{
			ObjectList* args = dynamic_cast<ObjectList*>(method.args);
			if (args == null)
				throw AuthException("None args(need username and password)");
			try{
				String authToken = authChecker->login(*args);
				throw LoginException(authToken, "login success");
			}catch (LoginException&){
				throw;
			}catch (Exception& e){
				throw AuthException(e.toString());
			}
		}
	}
	throw AuthException("You do not have permission to access this method");
}

String RpcConnection::login(const ObjectList& args)
{
	RpcMethod req("login", const_cast<ObjectList*>(&args), true);
	req.status = RpcMethod::STATUS_REQUEST_LOGIN;
	RpcMethod methodResult;
	sendCall4Result(req, methodResult);
	if (methodResult.status == RpcMethod::STATUS_RESPONSE_OK){
		this->authToken = methodResult.authToken;
		methodResult.releaseResult();
		return authToken;
	}
	else{
		String msg = methodResult.getResultStringAndRelease();
		throw RpcCallException(methodResult.status, msg);
	}
}

String RpcConnection::logout()
{
	RpcMethod req("logout", null, true, authToken);
	req.status = RpcMethod::STATUS_REQUEST_LOGOUT;
	RpcMethod methodResult;
	sendCall4Result(req, methodResult);
	if (methodResult.status == RpcMethod::STATUS_RESPONSE_OK){
		this->authToken = "";
		return methodResult.getResultStringAndRelease();
	}
	else{
		String msg = methodResult.getResultStringAndRelease();
		throw RpcCallException(methodResult.status, msg);
	}
}

RpcSerializeable* RpcConnection::getSerializer(cstring name)
{
	if(strlen(name) == 0)
		name = serializerType.c_str();
	RpcSerializeable* serializer = RpcSerializerFactory::instance().getSerializer(name);
	if (serializer == null)
	{
		String msg = String::format("have not found serializer '%s'", name);
		throw SerializeException(msg);
	}
	return serializer;
}

void RpcConnection::notifyEvent(cstring event, Object* sender,
	const ObjectList& args)
{
	ObjectList argsWithEvent;
	argsWithEvent.lease(args); // argsWithEvent will not be auto deleted!
	ScopePointer<Object> eventObject = toObject(event);
	(void)argsWithEvent.insert(0, eventObject);//event
	onNotifyEvent(event, argsWithEvent);
}

void RpcConnection::onNotifyEvent(cstring event, const ObjectList& args)
{
	return;
}

bool RpcConnection::subscribe(cstring event, cstring method)
{
	return false;
}

bool RpcConnection::unsubscribe(cstring event, cstring method)
{
	return false;
}

void RpcConnection::setDataHookHandler(RpcDataHookHandler* handler)
{
	return;
}

}//end of namespace brpc
