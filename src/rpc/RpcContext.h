#pragma once
#include "stdafx.h"
#include "Object.h"
#include "ObjectList.h"
#include "RpcSerializeable.h"

namespace bluemei{

/*
* RPC远程调用环境
* @author 李章梅
* @create 2014/7/13
*/
class RpcContext : public virtual Object
{
public:
	RpcContext(){}
	virtual ~RpcContext(){}	
public:
	virtual Object* call(cstring name, const ObjectList& args) =0;
	virtual void cast(cstring name, const ObjectList& args) =0;

	virtual Object* call(cstring obj, cstring name, const ObjectList& args) =0;
	virtual void cast(cstring obj, cstring name, const ObjectList& args) =0;

public:
	virtual void notifyEvent(cstring event, Object* sender, const ObjectList& args) =0;

	virtual bool subscribe(cstring event, cstring method) =0;
	virtual bool unsubscribe(cstring event, cstring method) =0;
public:
	virtual String login(const ObjectList& args)=0;
	virtual String logout()=0;

public:
	typedef HashMap<String,String> Headers;
	virtual bool inputCall(const Headers& headers, const InputStream& input) =0;
};


}//end of namespace bluemei