#pragma once
#include "stdafx.h"
#include "AnyFunction.h"
#include "FunctionWrapper.h"


namespace brpc{

//#define toObject(val) std::is_same<void, decltype(val)>::value
template<bool T>
struct returner;

template<>
struct returner<true>{
	template<typename T>
	inline static Object* invoke(T& f,const ObjectList& args){ 
		(void)f(args);
		return null;
	}
};

template<>
struct returner<false>{
	template<typename T>
	inline static Object* invoke(T& f,const ObjectList& args){ 
		return brpc::toObject(f(args)); 
	}
};

/*
* 函数半实现类
* @author Javeme
* @create 2014/7/5
*/
class ConcreteAnyFunction : public AnyFunction
{
public:
	ConcreteAnyFunction(cstring name=""):m_name(name) {}
	virtual ~ConcreteAnyFunction(){}
public:
	virtual cstring name() const { return m_name; }
	virtual void setName(cstring name){ this->m_name = name; }
private:
	String m_name;
};

/*
* 模板函数类
* @author Javeme
* @create 2014/7/5
*/
template <typename Func>
class TemplateFunction : public ConcreteAnyFunction
{
public:
	typedef TemplateFunction<Func> SelfType;

	TemplateFunction(cstring name, const FunctionWrapper<Func>& fw)
		:ConcreteAnyFunction(name),m_fw(fw){}
	virtual ~TemplateFunction(){}

public:
	virtual Object* operator()(const ObjectList& args)
	{
		typedef FunctionWrapper<Func>::RetType RetType;
		//if(std::is_same<void, RetType>::value)
		//	return null;
		//toObject(m_fw(args)
		return returner<std::is_same<void, RetType>::value>::invoke(m_fw,args);
	}

	virtual float matchArgsType(const ObjectList& args)
	{
		return m_fw.matchArgsType(args);
	}

	virtual int argsSize() const
	{
		return m_fw.argsSize();
	}
	
	virtual byte* address() const
	{
		union{decltype(m_fw.getFuncAddress()) funcAddress; byte* address;} addr;
		addr.funcAddress = m_fw.getFuncAddress();
		return addr.address;//reinterpret_cast<byte*>(m_fw.getFuncAddress())
	}

	virtual bool equals(const AnyFunction* other) const 
	{
		auto otherFun = dynamic_cast<const SelfType*>(other);
		if(otherFun == null)
			return false;

		return (address() == otherFun->address());
	}

	virtual String toString() const
	{
		//String paras = typeid(Func).name();
		List<cstring> paras = m_fw.paraTypes();
		String strParas;
		//for_each(paras.begin(), paras.end(), [&](cstring i){ strParas.append(i).append(','); });
		for (auto itor = paras.begin(); itor != paras.end(); ++itor)
		{
			strParas.append(*itor).append(",");
		}
		if (strParas.length() > 0)
			strParas = strParas.getLeftSub(strParas.length()-1);
		cstring retPara = m_fw.retParaType();
		String name = fullName();
		return String::format("%s %s(%s)", retPara, name.c_str(), strParas.c_str());
	}
	
	virtual String fullName() const 
	{ 
		if(m_fw.isClassFunc())
			return String::format("%s.%s",m_fw.getClassName(), name());
		else
			return name();
	}
private:
	FunctionWrapper<Func> m_fw;
};


}//end of namespace brpc