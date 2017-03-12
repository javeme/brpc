#pragma once
#include "src/util/BRpcUtil.h"


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
// This is just a FunctionWrapper demo of two number of args
template <typename T, typename Arg1, typename Arg2>
class FunctionWrapper< T(*)(Arg1, Arg2)>
{
public:
    typedef T FuncType(Arg1, Arg2);
    typedef T RetType;
    typedef FunctionWrapper< T(*)(Arg1, Arg2)> SelfType;

    FunctionWrapper(FuncType f) : m_fun(&f) {}
    virtual ~FunctionWrapper() {}

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
                "args size(%d) not matched size %d",args.size(),argsSize())));
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
    enum ArgsCounter{ I_Arg1, I_Arg2, ARGS_COUNT };

private:
    struct None;

    // the typename S is for avoiding error:
    // explicit specialization in non-namespace scope
    template<int INDEX, typename S=void>
    struct argsType{ typedef None Type; };

    template<typename S>
    struct argsType<0, S>{ typedef Arg1 Type; };

    template<typename S>
    struct argsType<1, S>{ typedef Arg2 Type; };

private:
    template<int INDEX, typename S=void>
    struct TypeChecker{
        static void check(const ObjectList& args, int& match){
            TypeChecker<INDEX-1>::check(args, match);
            valueOf<typename argsType<INDEX>::Type>(args[INDEX]);
            match++;
        }
    };
    template<typename S>
    struct TypeChecker<-1, S>{
        static void check(const ObjectList& args, int& match){}
    };

private:
    template<int INDEX, typename S=void>
    struct ArgTypeGetter{
        static void get(List<cstring>& types){
            ArgTypeGetter<INDEX-1>::get(types);
            types.push_back(type2name(typename argsType<INDEX>::Type));
        }
    };
    template<typename S>
    struct ArgTypeGetter<-1, S>{
        static void get(List<cstring>& types){}
    };
};


template <typename C, typename R>
class FunctionWrapper<R(C::*)()> {
public:
    typedef R RetType;
    typedef RetType (C::*FuncType)();
    typedef FunctionWrapper<R(C::*)()> SelfType;

    FunctionWrapper(FuncType f) : m_fun(f) {}
    virtual ~FunctionWrapper() {}

    virtual unsigned int argsSize() const{ return ARGS_COUNT; }
    virtual int matchArgsType(const ObjectList& args) const {
        return MatchArgsTypeHelper<ARGS_COUNT>::matchArgsType(args);
    }
    RetType operator()(const ObjectList& args) {
        if (args.size() < argsSize()) {
            throw ArgNotMatchedException(String::format(
                "args size(%d) not matched size %d", args.size(), argsSize()));
        }
        const static int CLS_INDEX = ArgsCounter::I_C;
        C* _this = valueOf<typename argsType<CLS_INDEX>::Type>(args[CLS_INDEX]);
        checkNullPtr(_this);
        return (_this->*m_fun)();
    }
    virtual FuncType getFuncAddress() const{ return m_fun; }
    virtual cstring retParaType() const{ return type2name(RetType); }
    virtual vector<cstring> paraTypes() const {
        vector<cstring> types;
        ArgTypeGetter<ARGS_COUNT-1>::get(types);
        return types;
    }
private:
    FuncType m_fun;
private:
    enum ArgsCounter{ I_C, ARGS_COUNT };
private:
    struct None;
    template<int INDEX, typename S=void>
    struct argsType{ typedef None Type; };

    template<typename S>
    struct argsType<ArgsCounter::I_C, S>{ typedef C* Type; };

private:
    template<int INDEX, typename S=void>
    struct TypeChecker{
        static void check(const ObjectList& args, int& match){
            TypeChecker<INDEX-1>::check(args, match);
            valueOf<typename argsType<INDEX>::Type>(args[INDEX]);
            match++;
        }
    };

    template<typename S>
    struct TypeChecker<-1, S>{
        static void check(const ObjectList& args, int& match){}
    };
private:
    template<int INDEX, typename S=void>
    struct ArgTypeGetter{
        static void get(vector<cstring>& types){
            ArgTypeGetter<INDEX-1>::get(types);
            types.push_back(type2name(typename argsType<INDEX>::Type));
        }
    };

    template<typename S>
    struct ArgTypeGetter<-1, S>{ static void get(vector<cstring>& types){} };
private:
    template<int ARGS_SIZE, typename S=void>
    struct MatchArgsTypeHelper{
        static int matchArgsType(const ObjectList& args){
            int match = 0;
            try{
                TypeChecker<ARGS_SIZE-1>::check(args, match);
            }catch (Exception& ){}
            return match/ARGS_SIZE;
        }
    };

    template<typename S>
    struct MatchArgsTypeHelper<0, S>{
        static int matchArgsType(const ObjectList& args){
            if (args.size() > 0) {
                return 0;
            }
            return 1;
        }
    };
};
*/


#define VA_ARGS(...)  __VA_ARGS__

#define JOIN(LEFT, RIGHT)  LEFT##RIGHT

#define TYPENAME(T)  typename T

// index of ArgType
#define INDEX_NAME(T)  JOIN(I_, T)
#define ARG_INDEX(T)  ((int) INDEX_NAME(T)) /*ArgsCounter::INDEX_NAME(T)*/

#define VALUE_OF_ARG(T) \
    methodArg<T>(args[ARG_INDEX(T)])
// end of VALUE_OF_ARG

#ifdef WIN32
// msvc supports full member template specialization inside of the class body,
// we use it and also avoiding vs2010-bug-of-partial-template-specialization:
// https://www.pcreview.co.uk/threads/vs2010-bug-of-partial-template-specialization.4071879/
#define DECLARE_PARTIAL_TEMPLATE

#define ARG_TYPE(T) \
    template<> struct argsType<ARG_INDEX(T)>{ typedef T Type; }
// end of ARG_TYPE

#define CLS_TYPE(C) \
    template<> struct argsType<ARG_INDEX(C)>{ typedef C* Type; }
// end of CLS_TYPE

#else

// must define the full specialization of member templates
// outside of the class body with gcc
#define DECLARE_PARTIAL_TEMPLATE , typename S=void

#define ARG_TYPE(T) \
    template<typename S> struct argsType<ARG_INDEX(T), S>{ typedef T Type; }
// end of ARG_TYPE

#define CLS_TYPE(C) \
    template<typename S> struct argsType<ARG_INDEX(C), S>{ typedef C* Type; }
// end of CLS_TYPE

#endif // end of WIN32


//函数指针包装类 宏
#define FUN_OF_ARGS(R, TYPENAME_ARGS, R_ARGS,                                 \
                    ARGS, ARGS_INDEX, ARG_TYPES, VALUE_OF_ARGS)               \
template <typename R/*,*/ TYPENAME_ARGS>                                      \
class FunctionWrapper<R_ARGS>                                                 \
{                                                                             \
public:                                                                       \
    typedef R RetType;                                                        \
    typedef RetType (*FuncType)(ARGS);                                        \
    typedef FunctionWrapper<R_ARGS> SelfType;                                 \
                                                                              \
    FunctionWrapper(FuncType f) : m_fun(f) {}                                 \
    virtual ~FunctionWrapper() {}                                             \
                                                                              \
    virtual bool isClassFunc() const{ return false; }                         \
    virtual cstring getClassName() const{ return ""; }                        \
    virtual unsigned int argsSize() const{ return ARGS_COUNT; }               \
                                                                              \
    virtual float matchArgsType(const ObjectList& args) const                 \
    {                                                                         \
        return MatchArgsTypeHelper<ARGS_COUNT>::matchArgsType(args);          \
    }                                                                         \
                                                                              \
    RetType operator()(const ObjectList& args)                                \
    {                                                                         \
        if (args.size() < argsSize())                                         \
        {                                                                     \
            throwpe(ArgNotMatchedException(String::format(                    \
                "args size(%d) not matched size %d",args.size(),argsSize())));\
        }                                                                     \
                                                                              \
        return (*m_fun)(VALUE_OF_ARGS);                                       \
    }                                                                         \
                                                                              \
    virtual FuncType getFuncAddress() const{ return m_fun; }                  \
                                                                              \
    virtual cstring retParaType() const{ return type2name(RetType); }         \
                                                                              \
    virtual List<cstring> paraTypes() const                                   \
    {                                                                         \
        List<cstring> types;                                                  \
        ArgTypeGetter<ARGS_COUNT-1>::get(types);                              \
        return types;                                                         \
    }                                                                         \
                                                                              \
private:                                                                      \
    FuncType m_fun;                                                           \
                                                                              \
private:                                                                      \
    enum ArgsCounter{ ARGS_INDEX/*,*/ ARGS_COUNT };                           \
                                                                              \
private:                                                                      \
    struct None;                                                              \
                                                                              \
    template<int INDEX DECLARE_PARTIAL_TEMPLATE>                              \
    struct argsType{ typedef None Type; };                                    \
                                                                              \
    ARG_TYPES;                                                                \
                                                                              \
private:                                                                      \
    template<int INDEX, typename S=void>                                      \
    struct TypeChecker{                                                       \
        static void check(const ObjectList& args, float& match){              \
            TypeChecker<INDEX-1>::check(args, match);                         \
            typedef typename argsType<INDEX>::Type ArgType;                   \
            MatchLevel l = matchLevel<ArgType>(args[INDEX]);                  \
            switch(l)                                                         \
            {                                                                 \
            case LEVEL_MATCHED:                                               \
                match = match + (1 + 1.0f / ARGS_COUNT);                      \
                break;                                                        \
            case LEVEL_CAN_CONVERT:                                           \
                match = match + 1;                                            \
                break;                                                        \
            case LEVEL_NOT_MATCHED:                                           \
            default:                                                          \
                break;                                                        \
            }                                                                 \
        }                                                                     \
    };                                                                        \
    template<typename S>                                                      \
    struct TypeChecker<-1, S>{                                                \
        static void check(const ObjectList& args, float& match){}             \
    };                                                                        \
                                                                              \
private:                                                                      \
    template<int INDEX, typename S=void>                                      \
    struct ArgTypeGetter{                                                     \
        static void get(List<cstring>& types){                                \
            ArgTypeGetter<INDEX-1>::get(types);                               \
            types.push_back(type2name(typename argsType<INDEX>::Type));       \
        }                                                                     \
    };                                                                        \
    template<typename S>                                                      \
    struct ArgTypeGetter<-1, S>{                                              \
        static void get(List<cstring>& types){}                               \
    };                                                                        \
                                                                              \
private:                                                                      \
    template<int ARGS_SIZE, typename S=void>                                  \
    struct MatchArgsTypeHelper{                                               \
        static float matchArgsType(const ObjectList& args){                   \
            float match = 0;                                                  \
            if (args.size() != ARGS_SIZE)                                     \
                return 0;                                                     \
            TypeChecker<ARGS_SIZE-1>::check(args, match);                     \
            return match / ARGS_SIZE;                                         \
        }                                                                     \
    };                                                                        \
    template<typename S>                                                      \
    struct MatchArgsTypeHelper<0, S>{                                         \
        static float matchArgsType(const ObjectList& args){                   \
            if (args.size() > 0)                                              \
            {                                                                 \
                return 0;                                                     \
            }                                                                 \
            return 1;                                                         \
        }                                                                     \
    };                                                                        \
};                                                                            \
//end of FUN_OF_ARGS


//类成员函数指针包装类 宏
#define CLS_FUN_OF_ARGS(C, R, TYPENAME_ARGS, R_ARGS,                          \
                        ARGS, ARGS_INDEX, ARG_TYPES, VALUE_OF_ARGS, MAY_CONST)\
template <typename C, typename R/*,*/ TYPENAME_ARGS>                          \
class FunctionWrapper<R_ARGS MAY_CONST>                                       \
{                                                                             \
public:                                                                       \
    typedef R RetType;                                                        \
    typedef C ClassType;                                                      \
    typedef RetType (ClassType::*FuncType)(ARGS) MAY_CONST;                   \
    typedef FunctionWrapper<R_ARGS> SelfType;                                 \
                                                                              \
    FunctionWrapper(FuncType f) : m_fun(f){}                                  \
    virtual ~FunctionWrapper() {}                                             \
                                                                              \
    virtual bool isClassFunc() const{ return true; }                          \
    virtual cstring getClassName() const{ return type2name(ClassType); }      \
    virtual unsigned int argsSize() const{ return ARGS_COUNT; }               \
                                                                              \
    virtual float matchArgsType(const ObjectList& args) const                 \
    {                                                                         \
        return MatchArgsTypeHelper<ARGS_COUNT>::matchArgsType(args);          \
    }                                                                         \
                                                                              \
    RetType operator()(const ObjectList& args)                                \
    {                                                                         \
        if (args.size() < argsSize())                                         \
        {                                                                     \
            throwpe(ArgNotMatchedException(String::format(                    \
                "args size(%d) not matched size %d",args.size(),argsSize())));\
        }                                                                     \
                                                                              \
        const static int CLS_INDEX = ARG_INDEX(C);                            \
        /*typedef typename argsType<CLS_INDEX>::Type ClassType;*/             \
        ClassType* _this = methodArg<ClassType*>(args[CLS_INDEX]);            \
        checkNullPtr(_this);                                                  \
        return (_this->*m_fun)(VALUE_OF_ARGS);                                \
    }                                                                         \
                                                                              \
    virtual FuncType getFuncAddress() const{ return m_fun; }                  \
                                                                              \
    virtual cstring retParaType() const{ return type2name(RetType); }         \
                                                                              \
    virtual List<cstring> paraTypes() const                                   \
    {                                                                         \
        List<cstring> types;                                                  \
        ArgTypeGetter<ARGS_COUNT-1>::get(types);                              \
        return types;                                                         \
    }                                                                         \
                                                                              \
private:                                                                      \
    FuncType m_fun;                                                           \
                                                                              \
private:                                                                      \
    enum ArgsCounter{ ARGS_INDEX/*,*/ ARGS_COUNT };                           \
                                                                              \
private:                                                                      \
    struct None;                                                              \
                                                                              \
    template<int INDEX DECLARE_PARTIAL_TEMPLATE>                              \
    struct argsType{ typedef None Type; };                                    \
                                                                              \
    CLS_TYPE(C);                                                              \
    ARG_TYPES;                                                                \
                                                                              \
private:                                                                      \
    template<int INDEX, typename S=void>                                      \
    struct TypeChecker{                                                       \
        static void check(const ObjectList& args, float& match){              \
            TypeChecker<INDEX-1>::check(args, match);                         \
            typedef typename argsType<INDEX>::Type ArgType;                   \
            MatchLevel l = matchLevel<ArgType>(args[INDEX]);                  \
            switch(l)                                                         \
            {                                                                 \
            case LEVEL_MATCHED:                                               \
                match = match + (1 + 1.0f / ARGS_COUNT);                      \
                break;                                                        \
            case LEVEL_CAN_CONVERT:                                           \
                match = match + 1;                                            \
                break;                                                        \
            case LEVEL_NOT_MATCHED:                                           \
            default:                                                          \
                break;                                                        \
            }                                                                 \
        }                                                                     \
    };                                                                        \
    template<typename S>                                                      \
    struct TypeChecker<-1, S>{                                                \
        static void check(const ObjectList& args, float& match){}             \
    };                                                                        \
                                                                              \
private:                                                                      \
    template<int INDEX, typename S=void>                                      \
    struct ArgTypeGetter{                                                     \
        static void get(List<cstring>& types){                                \
            ArgTypeGetter<INDEX-1>::get(types);                               \
            types.push_back(type2name(typename argsType<INDEX>::Type));       \
        }                                                                     \
    };                                                                        \
    template<typename S>                                                      \
    struct ArgTypeGetter<-1, S>{                                              \
        static void get(List<cstring>& types){}                               \
    };                                                                        \
                                                                              \
private:                                                                      \
    template<int ARGS_SIZE, typename S=void>                                  \
    struct MatchArgsTypeHelper{                                               \
        static float matchArgsType(const ObjectList& args){                   \
            float match = 0;                                                  \
            if (args.size() != ARGS_SIZE)                                     \
                return 0;                                                     \
            TypeChecker<ARGS_SIZE-1>::check(args, match);                     \
            return match / ARGS_SIZE;                                         \
        }                                                                     \
    };                                                                        \
    template<typename S>                                                      \
    struct MatchArgsTypeHelper<0, S>{                                         \
        static float matchArgsType(const ObjectList& args){                   \
            if (args.size() > 0)                                              \
            {                                                                 \
                return 0;                                                     \
            }                                                                 \
            return 1;                                                         \
        }                                                                     \
    };                                                                        \
};                                                                            \
//end of CLS_FUN_OF_ARGS

}//end of namespace brpc
