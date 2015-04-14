#pragma once
#include "stdafx.h"
#include "Object.h"
#include "BRpcUtil.h"

namespace bluemei{

/*
* 函数包装类
* @author 李章梅
* @create 2014/7/5
*/
template <typename Func>
class FunctionWrapper;

//无参数函数
template <typename T>
class FunctionWrapper< T(*)()>
{
public:
	typedef T FuncType();
	typedef T RetType;
	typedef FunctionWrapper< T(*)()> SelfType;

	FunctionWrapper(FuncType f) : m_fun(&f){}

	virtual unsigned int argsSize() const{ return ARGS_COUNT; }

	virtual int matchArgsType(const ObjectList& args)
	{
		if (args.size() > argsSize())
		{
			return 0;
		}
		return 1;
	}

	RetType operator()(const ObjectList& args)
	{
		return (*m_fun)();
	}

	virtual FuncType* getFuncAddress() const{ return m_fun; }

	virtual cstring retParaType() const{ return typeid(RetType).name(); }

	virtual List<cstring> paraTypes() const{ return List<cstring>(); }

private:
	FuncType* m_fun;

private:
	struct ArgsCounter{ enum{COUNT}; };
	enum { ARGS_COUNT = ArgsCounter::COUNT};

private:
	struct None;

	template<int INDEX>
	struct argsType{ typedef None Type; };

private:
	template<int INDEX>
	struct TypeChecker{
		static void check(const ObjectList& args, int& match){
			TypeChecker<INDEX-1>::check(args, match);
			valueOf<argsType<INDEX>::Type>(args[INDEX]);
			match++;
		}
	};
	template<>
	struct TypeChecker<-1>{
		static void check(const ObjectList& args, int& match){}
	};
};

template <typename T>
class FunctionWrapper< T()>
{
public:
	typedef T FuncType();
	typedef T RetType;
	typedef FunctionWrapper< T()> SelfType;

	FunctionWrapper(FuncType f) : m_fun(&f){}

	virtual unsigned int argsSize() const{ return ARGS_COUNT; }

	virtual int matchArgsType(const ObjectList& args)
	{
		if (args.size() > argsSize())
		{
			return 0;
		}
		return 1;
	}

	RetType operator()(const ObjectList& args)
	{
		return (*m_fun)();
	}

	virtual FuncType* getFuncAddress() const{ return m_fun; }

	virtual cstring retParaType() const{ return typeid(RetType).name(); }

	virtual List<cstring> paraTypes() const{ return List<cstring>(); }

private:
	FuncType* m_fun;

private:
	struct ArgsCounter{ enum{COUNT}; };
	enum { ARGS_COUNT = ArgsCounter::COUNT};

private:
	struct None;

	template<int INDEX>
	struct argsType{ typedef None Type; };

private:
	template<int INDEX>
	struct TypeChecker{
		static void check(const ObjectList& args, int& match){
			TypeChecker<INDEX-1>::check(args, match);
			valueOf<argsType<INDEX>::Type>(args[INDEX]);
			match++;
		}
	};
	template<>
	struct TypeChecker<-1>{
		static void check(const ObjectList& args, int& match){}
	};
};

/*
template <typename T, typename Arg1, typename Arg2>
class FunctionWrapper< T(*)(Arg1, Arg2)>
{
public:
	typedef T FuncType(Arg1, Arg2);
	typedef T RetType;
	typedef FunctionWrapper< T(*)(Arg1, Arg2)> SelfType;

	FunctionWrapper(FuncType f) : m_fun(&f){}

	virtual unsigned int argsSize() const{ return ARGS_COUNT; }

	virtual int matchArgsType(const ObjectList& args)
	{
		int match = 0;
		try{
			TypeChecker<ARGS_COUNT-1>::check(args, match);
		}catch (Exception& e)
		{
			//e.printException();
		}
		return match/argsSize();
	}

	RetType operator()(const ObjectList& args)
	{
		if (args.size() < argsSize())
		{
			throwpe(ArgNotMatchedException(String::format(
				"args size(%d) not matched size %d",args.size() , argsSize())))
		}

		return (*m_fun)(valueOf<Arg1>(args[0]), valueOf<Arg2>(args[1]));
	}
private:
	FuncType* m_fun;

private:
	struct ArgsCounter{ enum{COUNT=2}; };
	enum { ARGS_COUNT = ArgsCounter::COUNT};

private:
	struct None;

	template<int INDEX>
	struct argsType{ typedef None Type; };

	template<>
	struct argsType<0>{ typedef Arg1 Type; };

	template<>
	struct argsType<1>{ typedef Arg2 Type; };

private:
	template<int INDEX>
	struct TypeChecker{
		static void check(const ObjectList& args, int& match){
			TypeChecker<INDEX-1>::check(args, match);
			valueOf<argsType<INDEX>::Type>(args[INDEX]);
			match++;
		}
	};
	template<>
	struct TypeChecker<-1>{
		static void check(const ObjectList& args, int& match){}
	};
};
*/

#define VA_ARGS(...) __VA_ARGS__

#define JOIN(LEFT, RIGHT) LEFT##RIGHT

#define TYPENAME_ARG(T) typename T

#define ARG(T) T

#define ARG_TYPE(T) \
template<> struct argsType<ArgsCounter::T>{ typedef T Type; }

#define VALUE_OF_ARG(T) \
valueOf<T>(args[ArgsCounter::T])


//函数指针包装类 宏
#define PFUN_OF_ARGS(TYPENAME_ARGS, ARGS, ARG_TYPES, VALUE_OF_ARGS)							 \
template <typename T, TYPENAME_ARGS>														 \
class FunctionWrapper< T(*)(ARGS)>															 \
{																							 \
public:																						 \
	typedef T RetType;																		 \
	typedef RetType FuncType(ARGS);															 \
	typedef FunctionWrapper< T(*)(ARGS)> SelfType;											 \
																							 \
	FunctionWrapper(FuncType f) : m_fun(&f){}												 \
																							 \
	virtual unsigned int argsSize() const{ return ARGS_COUNT; }								 \
																							 \
	virtual int matchArgsType(const ObjectList& args)										 \
	{																						 \
		int match = 0;																		 \
		try{																				 \
			TypeChecker<ARGS_COUNT-1>::check(args, match);									 \
		}catch (Exception& /*e*/)															 \
		{																					 \
			;																				 \
		}																					 \
		return match/argsSize();															 \
	}																						 \
																							 \
	RetType operator()(const ObjectList& args)												 \
	{																						 \
		if (args.size() < argsSize())														 \
		{																					 \
			throwpe(ArgNotMatchedException(String::format(									 \
				"args size(%d) not matched size %d",args.size() , argsSize())))				 \
		}																					 \
																							 \
		return (*m_fun)(VALUE_OF_ARGS);														 \
	}																						 \
																							 \
	virtual FuncType* getFuncAddress() const{ return m_fun; }								 \
																							 \
	virtual cstring retParaType() const{ return typeid(RetType).name(); }					 \
																							 \
	virtual List<cstring> paraTypes() const													 \
	{ 																						 \
		List<cstring> types;																 \
		ArgTypeGetter<ARGS_COUNT-1>::get(types);											 \
		return types; 																		 \
	}																						 \
																							 \
private:																					 \
	FuncType* m_fun;																		 \
																							 \
private:																					 \
	struct ArgsCounter{ enum{ARGS, COUNT}; };												 \
	enum { ARGS_COUNT = ArgsCounter::COUNT};												 \
																							 \
private:																					 \
	struct None;																			 \
																							 \
	template<int INDEX>																		 \
	struct argsType{ typedef None Type; };													 \
																							 \
	ARG_TYPES;																				 \
																							 \
private:																					 \
	template<int INDEX>																		 \
	struct TypeChecker{																		 \
		static void check(const ObjectList& args, int& match){								 \
			TypeChecker<INDEX-1>::check(args, match);										 \
			valueOf<argsType<INDEX>::Type>(args[INDEX]);									 \
			match++;																		 \
		}																					 \
	};																						 \
	template<>																				 \
	struct TypeChecker<-1>{																	 \
		static void check(const ObjectList& args, int& match){}								 \
	};																						 \
private:																					 \
	template<int INDEX>																		 \
	struct ArgTypeGetter{																	 \
		static void get(List<cstring>& types){												 \
			ArgTypeGetter<INDEX-1>::get(types);												 \
			types.push_back(typeid(argsType<INDEX>::Type).name());							 \
		}																					 \
	};																						 \
	template<>																				 \
	struct ArgTypeGetter<-1>{																 \
		static void get(List<cstring>& types){}												 \
	};																						 \
};																							 
	

//函数包装类 宏
#define FUN_OF_ARGS(TYPENAME_ARGS, ARGS, ARG_TYPES, VALUE_OF_ARGS)							 \
template <typename T, TYPENAME_ARGS>														 \
class FunctionWrapper< T(ARGS)>																 \
{																							 \
public:																						 \
	typedef T RetType;																		 \
	typedef RetType FuncType(ARGS);															 \
	typedef FunctionWrapper< T(ARGS)> SelfType;												 \
																							 \
	FunctionWrapper(FuncType f) : m_fun(&f){}												 \
																							 \
	virtual unsigned int argsSize() const{ return ARGS_COUNT; }								 \
																							 \
	virtual int matchArgsType(const ObjectList& args)										 \
	{																						 \
		int match = 0;																		 \
		try{																				 \
			TypeChecker<ARGS_COUNT-1>::check(args, match);									 \
		}catch (Exception& /*e*/)															 \
		{																					 \
			;																				 \
		}																					 \
		return match/argsSize();															 \
	}																						 \
																							 \
	RetType operator()(const ObjectList& args)												 \
	{																						 \
		if (args.size() < argsSize())														 \
		{																					 \
			throwpe(ArgNotMatchedException(String::format(									 \
				"args size(%d) not matched size %d",args.size() , argsSize())))				 \
		}																					 \
																							 \
		return (*m_fun)(VALUE_OF_ARGS);														 \
	}																						 \
																							 \
	virtual FuncType* getFuncAddress() const{ return m_fun; }								 \
																							 \
	virtual cstring retParaType() const{ return typeid(RetType).name(); }					 \
																							 \
	virtual List<cstring> paraTypes() const													 \
	{ 																						 \
		List<cstring> types;																 \
		ArgTypeGetter<ARGS_COUNT-1>::get(types);											 \
		return types; 																		 \
	}																						 \
																							 \
private:																					 \
	FuncType* m_fun;																		 \
																							 \
private:																					 \
	struct ArgsCounter{ enum{ARGS, COUNT}; };												 \
	enum { ARGS_COUNT = ArgsCounter::COUNT};												 \
																							 \
private:																					 \
	struct None;																			 \
																							 \
	template<int INDEX>																		 \
	struct argsType{ typedef None Type; };													 \
																							 \
	ARG_TYPES;																				 \
																							 \
private:																					 \
	template<int INDEX>																		 \
	struct TypeChecker{																		 \
		static void check(const ObjectList& args, int& match){								 \
			TypeChecker<INDEX-1>::check(args, match);										 \
			valueOf<argsType<INDEX>::Type>(args[INDEX]);									 \
			match++;																		 \
		}																					 \
	};																						 \
	template<>																				 \
	struct TypeChecker<-1>{																	 \
		static void check(const ObjectList& args, int& match){}								 \
	};																						 \
private:																					 \
	template<int INDEX>																		 \
	struct ArgTypeGetter{																	 \
		static void get(List<cstring>& types){												 \
			ArgTypeGetter<INDEX-1>::get(types);												 \
			types.push_back(typeid(argsType<INDEX>::Type).name());							 \
		}																					 \
	};																						 \
	template<>																				 \
	struct ArgTypeGetter<-1>{																 \
		static void get(List<cstring>& types){}												 \
	};																						 \
};																							 

#define FUN_AND_PFUN_OF_ARGS(TYPENAME_ARGS, ARGS, ARG_TYPES, VALUE_OF_ARGS) 				 \
	FUN_OF_ARGS(TYPENAME_ARGS, ARGS, ARG_TYPES, VALUE_OF_ARGS)								 \
	PFUN_OF_ARGS(TYPENAME_ARGS, ARGS, ARG_TYPES, VALUE_OF_ARGS)				


}//end of namespace bluemei
