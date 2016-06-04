#pragma once
#include "stdafx.h"
#include "ObjectList.h"
#include "RpcSerializeable.h"


namespace brpc{

/*
* RPCÔ¶³Ìµ÷ÓÃÆ÷
* @author Javeme
* @create 2014/7/13
*/
class RpcInvoker : public virtual Object
{
public:
	RpcInvoker(){}
	virtual ~RpcInvoker(){}	

	/****************************for client******************************/
public:
	virtual Object* call(cstring name, const ObjectList& args) =0;
	virtual void cast(cstring name, const ObjectList& args) =0;

	virtual Object* call(cstring obj, cstring name, const ObjectList& args) =0;
	virtual void cast(cstring obj, cstring name, const ObjectList& args) =0;
public:
	virtual bool subscribe(cstring event, cstring method) =0;
	virtual bool unsubscribe(cstring event, cstring method) =0;
public:
	virtual String login(const ObjectList& args)=0;
	virtual String logout()=0;

	/****************************for server******************************/
public:
	typedef HashMap<String,String> Headers;

	virtual bool invoke(const Headers& headers, const InputStream& input) =0;
	virtual void notifyEvent(cstring event, Object* sender, const ObjectList& args) =0;
};


}//end of namespace brpc