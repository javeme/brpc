#pragma once
#include "src/util/BRpcUtil.h"
#include "src/fun/AnyFunction.h"
#include "src/fun/TemplateFunction.h"
#include "src/fun/FunctionDefine.h"


namespace brpc{

/*
* 函数管理类
* @author Javeme
* @create 2014/7/5
*/
class FuncDispatcher : public VarContext
{
	typedef List<AnyFunction*> FuncList;
	typedef Map<std::string, FuncList> FuncMap;
	typedef Map<std::string, Object*> ObjMap;
	typedef List<FuncDispatcher*> DispatcherList;
public:
	FuncDispatcher(cstring name="dispatcher") : m_name(name){}
	virtual ~FuncDispatcher();

public:
	template <typename Func>
	bool registerFunction(cstring funcName, Func func)
	{
		checkNullPtr(funcName);
		String fname = funcName;
		if(fname.contain("&"))
			fname = fname.replace("&", "");
		if(fname.contain("::"))
			fname = fname.replace("::", ".");
		FunctionWrapper<Func> fw(func);
		TemplateFunction<Func>* tf = new TemplateFunction<Func>(fname, fw);
		bool success = addFunction(tf);
		if (!success)
			delete tf;
		return success;
	}
	#define regFunc(fun) registerFunction(_CODE2STRING(fun), fun)

	template <typename Func>
	void unregisterFunction(cstring funcName, Func func)
	{
		auto itor = m_funcMap.find(funcName);
		if (itor == m_funcMap.end())
		{
			return;
		}

		union{Func funcAddress; byte* address;} addr;
		addr.funcAddress = func;

		FuncList& funcList = itor->second;
		for(auto itor = funcList.begin(); itor != funcList.end(); )
		{
			if(addr.address == (*itor)->address())
			{
				itor = funcList.erase(itor);
			}
			else
				++itor;
		}
	}
	virtual void unregisterFunction(cstring funcName);
	virtual void clearAllFunction();

	virtual AnyFunction* getFunctionFromAll(cstring name, const ObjectList& args);

	virtual List<String> listFunctions() const;
	virtual FuncList findFunction(cstring name) const;
public:
	virtual bool registerVar(cstring name,Object* var);
	#define regVar(var) registerVar(_CODE2STRING(var), var)
	virtual bool unregisterVar(cstring name);
	virtual Object* getVar(cstring name) const;
	virtual List<String> listVars() const;
	virtual void clearAllVar();
	virtual Object* getVarFromAll(cstring name) const;
public:
	String name() const { return m_name; }
	void setName(cstring val) { m_name = val; }
public:
	virtual String functionsAsString() const;
	virtual String varsAsString() const;
	virtual String toString() const;
public:
	virtual Object* call(cstring name, const ObjectList& args);
	virtual Object* call(cstring obj, cstring name, const ObjectList& args);

	virtual bool extend(FuncDispatcher* dispatcher);
	virtual bool exclude(FuncDispatcher* dispatcher);
protected:
	virtual bool addFunction(AnyFunction* func);
	virtual bool hasFunction(cstring name) const;

	virtual AnyFunction* matchedFunction(cstring name, const ObjectList& args);
	virtual AnyFunction* matchedAllFunction(cstring name, const ObjectList& args);
private:
	FuncMap m_funcMap;
	ObjMap m_objMap;
	String m_name;

	DispatcherList m_extendDispatchers;
};

}//end of namespace brpc