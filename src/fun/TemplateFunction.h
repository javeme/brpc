#pragma once
#include "src/fun/AnyFunction.h"
#include "src/fun/FunctionWrapper.h"


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
		StringBuilder sb(64 + paras.size() * 16);

		// return type
		sb.append(m_fw.retParaType());
		sb.append(" ");

		// function name
		sb.append(this->fullName());

		// function paras
		sb.append("(");
		for (auto itor = paras.begin(); itor != paras.end(); ++itor)
		{
			sb.append(*itor).append(", ");
		}
		if (paras.size() > 0) // remove the last ", "
			sb.deleteSub(sb.length() - 2, sb.length());
		sb.append(")");

		return sb.toString();
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