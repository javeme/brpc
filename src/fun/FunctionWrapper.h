#pragma once
#include "stdafx.h"
#include "BRpcUtil.h"


namespace brpc{

/*
* 函数包装类
* @author Javeme
* @create 2014/7/5
*/
template <typename Func>
class FunctionWrapper;

#define type2name(t) typeid(t).name()

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
	virtual FuncType* getFuncAddress() const{ return m_fun; }								 
																							 
	virtual cstring retParaType() const{ return type2name(RetType); }					 
																							 
	virtual List<cstring> paraTypes() const													 
	{ 																						 
		List<cstring> types;																 
		ArgTypeGetter<ARGS_COUNT-1>::get(types);											 
		return types; 																		 
	}																						 
												
private:
	FuncType* m_fun;

private:
	struct ArgsCounter{ enum{Arg1, Arg2, COUNT}; };
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

private:															
	template<int INDEX>												
	struct ArgTypeGetter{											
		static void get(List<cstring>& types){						
			ArgTypeGetter<INDEX-1>::get(types);						
			types.push_back(type2name(argsType<INDEX>::Type));	
		}															
	};																
	template<>														
	struct ArgTypeGetter<-1>{										
		static void get(List<cstring>& types){}						
	};																
};


template <typename C, typename R > 
class FunctionWrapper<R(C::*)()> { 
public: 
	typedef R RetType; typedef RetType (C::*FuncType)();
	typedef FunctionWrapper<R(C::*)()> SelfType; 
	FunctionWrapper(FuncType f) : m_fun(f){} 
	virtual unsigned int argsSize() const{ return ARGS_COUNT; } 
	virtual int matchArgsType(const ObjectList& args) const { 
		return MatchArgsTypeHelper<ARGS_COUNT>::matchArgsType(args);
	} 
	RetType operator()(const ObjectList& args) { 
		if (args.size() < argsSize()) { { if(strcmp("ArgNotMatchedException(String::format( \"args size(%d) not matched size %d\",args.size() , argsSize()))","_ex")==0){ auto& _e=ArgNotMatchedException(String::format( "args size(%d) not matched size %d",args.size() , argsSize())); auto& __ex=_e; __ex.setPosition(64 ,__FSTREXP __FUNCTION__   ,"d:\\c++\\c++vs2010\\brpc\\brpc.cpp"); throw(_e); } else{ auto& _ex=ArgNotMatchedException(String::format( "args size(%d) not matched size %d",args.size() , argsSize())); auto& __ex=_ex; __ex.setPosition(64 ,__FSTREXP __FUNCTION__   ,"d:\\c++\\c++vs2010\\brpc\\brpc.cpp"); throw(_ex); }} } 
		const static int CLS_INDEX = ArgsCounter::C; 
		C* _this = valueOf<argsType<CLS_INDEX>::Type>(args[CLS_INDEX]); 
		checkNullPtr(_this); 
		return (_this->*m_fun)(); 
	}
	virtual FuncType getFuncAddress() const{ return m_fun; } 
	virtual cstring retParaType() const{ return type2name(RetType); } 
	virtual vector<cstring> paraTypes() const { vector<cstring> types; 
	ArgTypeGetter<ARGS_COUNT-1>::get(types); return types; } 
private: 
	FuncType m_fun; 
private: 
	struct ArgsCounter{ enum{C, COUNT}; }; 
	enum { ARGS_COUNT = ArgsCounter::COUNT}; 
private:
	struct None; 
	template<int INDEX> struct argsType{ typedef None Type; };
	template<> struct argsType<ArgsCounter::C>{ typedef C* Type; }; 
	;
private: 
	template<int INDEX> struct TypeChecker{ 
		static void check(const ObjectList& args, int& match){ 
			TypeChecker<INDEX-1>::check(args, match); 
			valueOf<argsType<INDEX>::Type>(args[INDEX]); 
			match++; 
		} 
	}; 
	template<> struct TypeChecker<-1>{ 
		static void check(const ObjectList& args, int& match){} 
	}; 
private: 
	template<int INDEX> struct ArgTypeGetter{ 
		static void get(vector<cstring>& types){ 
			ArgTypeGetter<INDEX-1>::get(types); 
			types.push_back(type2name(argsType<INDEX>::Type));
		} 
	}; 
	template<> struct ArgTypeGetter<-1>{ static void get(vector<cstring>& types){} }; 
private: 
	template<int ARGS_SIZE> 
	struct MatchArgsTypeHelper{ 
		static int matchArgsType(const ObjectList& args){ 
			int match = 0; 
			try{ TypeChecker<ARGS_SIZE-1>::check(args, match); }catch (Exception& ){ ; } 
			return match/ARGS_SIZE; 
		} 
	}; 
	template<> struct MatchArgsTypeHelper<0>{ static int matchArgsType(const ObjectList& args){ if (args.size() > 0) { return 0; } return 1; } }; 
};
*/

#define VA_ARGS(...) __VA_ARGS__

#define JOIN(LEFT, RIGHT) LEFT##RIGHT

#define TYPENAME_ARG(T) typename T

#define ARG(T) T

#define ARG_TYPE(T) \
template<> struct argsType<ArgsCounter::T>{ typedef T Type; }

#define VALUE_OF_ARG(T) \
methodArg<T>(args[ArgsCounter::T])


//函数指针包装类 宏
#define FUN_OF_ARGS(R, TYPENAME_ARGS, R_ARGS, ARGS, INDEX_ARGS, ARG_TYPES, VALUE_OF_ARGS)	 \
template <typename R/*,*/ TYPENAME_ARGS>													 \
class FunctionWrapper<R_ARGS>																 \
{																							 \
public:																						 \
	typedef R RetType;																		 \
	typedef RetType (*FuncType)(ARGS);														 \
	typedef FunctionWrapper<R_ARGS> SelfType;												 \
																							 \
	FunctionWrapper(FuncType f) : m_fun(f){}												 \
																							 \
	virtual bool isClassFunc() const{ return false; }										 \
	virtual cstring getClassName() const{ return ""; }										 \
	virtual unsigned int argsSize() const{ return ARGS_COUNT; }								 \
																							 \
	virtual float matchArgsType(const ObjectList& args) const								 \
	{																						 \
		return MatchArgsTypeHelper<ARGS_COUNT>::matchArgsType(args);						 \
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
	virtual FuncType getFuncAddress() const{ return m_fun; }								 \
																							 \
	virtual cstring retParaType() const{ return type2name(RetType); }						 \
																							 \
	virtual List<cstring> paraTypes() const													 \
	{ 																						 \
		List<cstring> types;																 \
		ArgTypeGetter<ARGS_COUNT-1>::get(types);											 \
		return types; 																		 \
	}																						 \
																							 \
private:																					 \
	FuncType m_fun;																			 \
																							 \
private:																					 \
	struct ArgsCounter{ enum{INDEX_ARGS/*,*/ COUNT}; };										 \
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
		static void check(const ObjectList& args, float& match){							 \
			TypeChecker<INDEX-1>::check(args, match);										 \
			MatchLevel l = matchLevel<argsType<INDEX>::Type>(args[INDEX]);					 \
			switch(l)																		 \
			{																				 \
			case LEVEL_MATCHED:																 \
				match = match+(1 + 1.0f/ARGS_COUNT);										 \
				break;																		 \
			case LEVEL_CAN_CONVERT:															 \
				match = match+1;															 \
				break;																		 \
			case LEVEL_NOT_MATCHED:															 \
			default:																		 \
				break;																		 \
			}																				 \
		}																					 \
	};																						 \
	template<>																				 \
	struct TypeChecker<-1>{																	 \
		static void check(const ObjectList& args, float& match){}							 \
	};																						 \
																							 \
private:																					 \
	template<int INDEX>																		 \
	struct ArgTypeGetter{																	 \
		static void get(List<cstring>& types){												 \
			ArgTypeGetter<INDEX-1>::get(types);												 \
			types.push_back(type2name(argsType<INDEX>::Type));								 \
		}																					 \
	};																						 \
	template<>																				 \
	struct ArgTypeGetter<-1>{																 \
		static void get(List<cstring>& types){}												 \
	};																						 \
																							 \
private:																					 \
	template<int ARGS_SIZE>																	 \
	struct MatchArgsTypeHelper{																 \
		static float matchArgsType(const ObjectList& args){									 \
			float match = 0;																 \
			if (args.size() != ARGS_SIZE)													 \
				return 0;																	 \
			TypeChecker<ARGS_SIZE-1>::check(args, match);									 \
			return match/ARGS_SIZE;															 \
		}																					 \
	};																						 \
	template<>																				 \
	struct MatchArgsTypeHelper<0>{															 \
		static float matchArgsType(const ObjectList& args){									 \
			if (args.size() > 0)															 \
			{																				 \
				return 0;																	 \
			}																				 \
			return 1;																		 \
		}																					 \
	};																						 \
};																							 


//类成员函数指针包装类 宏
#define CLS_FUN_OF_ARGS(C, R, TYPENAME_ARGS, R_ARGS, ARGS, INDEX_ARGS, ARG_TYPES, VALUE_OF_ARGS, MAY_CONST)\
template <typename C, typename R/*,*/ TYPENAME_ARGS>										 \
class FunctionWrapper<R_ARGS MAY_CONST>														 \
{																							 \
public:																						 \
	typedef R RetType;																		 \
	typedef C ClassType;																	 \
	typedef RetType (ClassType::*FuncType)(ARGS) MAY_CONST;									 \
	typedef FunctionWrapper<R_ARGS> SelfType;												 \
																							 \
	FunctionWrapper(FuncType f) : m_fun(f){}												 \
																							 \
	virtual bool isClassFunc() const{ return true; }										 \
	virtual cstring getClassName() const{ return type2name(ClassType); }					 \
	virtual unsigned int argsSize() const{ return ARGS_COUNT; }								 \
																							 \
	virtual float matchArgsType(const ObjectList& args) const								 \
	{																						 \
		return MatchArgsTypeHelper<ARGS_COUNT>::matchArgsType(args);						 \
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
		const static int CLS_INDEX = ArgsCounter::C;										 \
		C* _this = methodArg<argsType<CLS_INDEX>::Type>(args[CLS_INDEX]);					 \
		checkNullPtr(_this);																 \
		return (_this->*m_fun)(VALUE_OF_ARGS);												 \
	}																						 \
																							 \
	virtual FuncType getFuncAddress() const{ return m_fun; }								 \
																							 \
	virtual cstring retParaType() const{ return type2name(RetType); }						 \
																							 \
	virtual List<cstring> paraTypes() const													 \
	{ 																						 \
		List<cstring> types;																 \
		ArgTypeGetter<ARGS_COUNT-1>::get(types);											 \
		return types; 																		 \
	}																						 \
																							 \
private:																					 \
	FuncType m_fun;																			 \
																							 \
private:																					 \
	struct ArgsCounter{ enum{INDEX_ARGS/*,*/ COUNT}; };										 \
	enum { ARGS_COUNT = ArgsCounter::COUNT};												 \
																							 \
private:																					 \
	struct None;																			 \
																							 \
	template<int INDEX>																		 \
	struct argsType{ typedef None Type; };													 \
																							 \
	template<> struct argsType<ArgsCounter::C>{ typedef C* Type; };							 \
																							 \
	ARG_TYPES;																				 \
																							 \
private:																					 \
	template<int INDEX>																		 \
	struct TypeChecker{																		 \
		static void check(const ObjectList& args, float& match){							 \
			TypeChecker<INDEX-1>::check(args, match);										 \
			MatchLevel l = matchLevel<argsType<INDEX>::Type>(args[INDEX]);					 \
			switch(l)																		 \
			{																				 \
			case LEVEL_MATCHED:																 \
				match = match+(1 + 1.0f/ARGS_COUNT);										 \
				break;																		 \
			case LEVEL_CAN_CONVERT:															 \
				match = match+1;															 \
				break;																		 \
			case LEVEL_NOT_MATCHED:															 \
			default:																		 \
				break;																		 \
			}																				 \
		}																					 \
	};																						 \
	template<>																				 \
	struct TypeChecker<-1>{																	 \
		static void check(const ObjectList& args, float& match){}							 \
	};																						 \
																							 \
private:																					 \
	template<int INDEX>																		 \
	struct ArgTypeGetter{																	 \
		static void get(List<cstring>& types){												 \
			ArgTypeGetter<INDEX-1>::get(types);												 \
			types.push_back(type2name(argsType<INDEX>::Type));								 \
		}																					 \
	};																						 \
	template<>																				 \
	struct ArgTypeGetter<-1>{																 \
		static void get(List<cstring>& types){}												 \
	};																						 \
																							 \
private:																					 \
	template<int ARGS_SIZE>																	 \
	struct MatchArgsTypeHelper{																 \
		static float matchArgsType(const ObjectList& args){									 \
			float match = 0;																 \
			if (args.size() != ARGS_SIZE)													 \
				return 0;																	 \
			TypeChecker<ARGS_SIZE-1>::check(args, match);									 \
			return match/ARGS_SIZE;															 \
		}																					 \
	};																						 \
	template<>																				 \
	struct MatchArgsTypeHelper<0>{															 \
		static float matchArgsType(const ObjectList& args){									 \
			if (args.size() > 0)															 \
			{																				 \
				return 0;																	 \
			}																				 \
			return 1;																		 \
		}																					 \
	};																						 \
};																							 


}//end of namespace brpc
