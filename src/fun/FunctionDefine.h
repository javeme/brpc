#pragma once
#include "src/fun/FunctionWrapper.h"
#include "src/fun/mfor.h"


namespace brpc{

/*
FUN_AND_PFUN_OF_ARGS(
    VA_ARGS(TYPENAME_ARG(Arg1)),
    VA_ARGS(         ARG(Arg1)),
    VA_ARGS(    ARG_TYPE(Arg1)),
    VA_ARGS(VALUE_OF_ARG(Arg1))
    )

FUN_AND_PFUN_OF_ARGS(
    VA_ARGS(TYPENAME_ARG(Arg1), TYPENAME_ARG(Arg2)),
    VA_ARGS(         ARG(Arg1),          ARG(Arg2)),
    VA_ARGS(    ARG_TYPE(Arg1);     ARG_TYPE(Arg2)),
    VA_ARGS(VALUE_OF_ARG(Arg1), VALUE_OF_ARG(Arg2))
    )
*/


#define _TYPENAME_ARGS1  ,typename Arg1
#define _TYPENAME_ARGS(n, m)  m, typename JOIN(Arg, n)

// NOTE: FORA3(_ARGS) => _ARGS(3, FORA2(_ARGS)) => FORA2(_ARGS), Arg3
//    => _ARGS(2, FORA1(_ARGS)), Arg3 => FORA1(_ARGS), Arg2, Arg3
//    => _ARGS1, Arg2, Arg3 => Arg1, Arg2, Arg3
#define _ARG_LIST1  Arg1
#define _ARG_LIST(n, m)  m, JOIN(Arg, n)

#define _ARGS_INDEX1  INDEX_NAME(Arg1),
#define _ARGS_INDEX(n, m)  m INDEX_NAME(JOIN(Arg, n)),

#define _ARGS_TYPE1  ARG_TYPE(Arg1)
#define _ARGS_TYPE(n, m)  m; ARG_TYPE(JOIN(Arg, n))

#define _VALUE_OF_ARGS1  VALUE_OF_ARG(Arg1)
#define _VALUE_OF_ARGS(n, m)  m, VALUE_OF_ARG(JOIN(Arg, n))

#define _FUN_R_ARGS(R, ARGS)  R(ARGS)
#define _PFN_R_ARGS(R, ARGS)  R(*)(ARGS)
#define _CPFRARGS(C, R, ARGS)  R(C::*)(ARGS)

#define FUNC_OF_ARGS(n)                                                       \
    FUN_OF_ARGS(                                                              \
        VA_ARGS(                                          R),                 \
        VA_ARGS(              JOIN(FORA, n)( _TYPENAME_ARGS)),                \
        VA_ARGS(_FUN_R_ARGS(R,JOIN(FORA, n)(      _ARG_LIST))),               \
        VA_ARGS(              JOIN(FORA, n)(      _ARG_LIST)),                \
        VA_ARGS(              JOIN(FORA, n)(    _ARGS_INDEX)),                \
        VA_ARGS(              JOIN(FORA, n)(     _ARGS_TYPE)),                \
        VA_ARGS(              JOIN(FORA, n)( _VALUE_OF_ARGS))                 \
        )

#define PFUNC_OF_ARGS(n)                                                      \
    FUN_OF_ARGS(                                                              \
        VA_ARGS(                                          R),                 \
        VA_ARGS(              JOIN(FORA, n)( _TYPENAME_ARGS)),                \
        VA_ARGS(_PFN_R_ARGS(R,JOIN(FORA, n)(      _ARG_LIST))),               \
        VA_ARGS(              JOIN(FORA, n)(      _ARG_LIST)),                \
        VA_ARGS(              JOIN(FORA, n)(    _ARGS_INDEX)),                \
        VA_ARGS(              JOIN(FORA, n)(     _ARGS_TYPE)),                \
        VA_ARGS(              JOIN(FORA, n)( _VALUE_OF_ARGS))                 \
        )

#define CLS_PF_OF_ARGS(n)                                                     \
    CLS_FUN_OF_ARGS(                                                          \
        VA_ARGS(                                          C),                 \
        VA_ARGS(                                          R),                 \
        VA_ARGS(              JOIN(FORA, n)( _TYPENAME_ARGS)),                \
        VA_ARGS(_CPFRARGS(C,R,JOIN(FORA, n)(      _ARG_LIST))),               \
        VA_ARGS(              JOIN(FORA, n)(      _ARG_LIST)),                \
        VA_ARGS(INDEX_NAME(C),JOIN(FORA, n)(    _ARGS_INDEX)),                \
        VA_ARGS(              JOIN(FORA, n)(     _ARGS_TYPE)),                \
        VA_ARGS(              JOIN(FORA, n)( _VALUE_OF_ARGS)),                \
        VA_ARGS(                                           )                  \
        )

#define CONST_CLS_PF_OF_ARGS(n)                                               \
    CLS_FUN_OF_ARGS(                                                          \
        VA_ARGS(                                          C),                 \
        VA_ARGS(                                          R),                 \
        VA_ARGS(              JOIN(FORA, n)( _TYPENAME_ARGS)),                \
        VA_ARGS(_CPFRARGS(C,R,JOIN(FORA, n)(      _ARG_LIST))),               \
        VA_ARGS(              JOIN(FORA, n)(      _ARG_LIST)),                \
        VA_ARGS(INDEX_NAME(C),JOIN(FORA, n)(    _ARGS_INDEX)),                \
        VA_ARGS(              JOIN(FORA, n)(     _ARGS_TYPE)),                \
        VA_ARGS(              JOIN(FORA, n)( _VALUE_OF_ARGS)),                \
        VA_ARGS(                                      const)                  \
        )

#define DEFINE_FUNC_OF_ARGS(n)                                                \
    FUNC_OF_ARGS(n)                                                           \
    PFUNC_OF_ARGS(n)                                                          \
    CLS_PF_OF_ARGS(n)                                                         \
    CONST_CLS_PF_OF_ARGS(n)


//define function wrappers
DEFINE_FUNC_OF_ARGS(0);
DEFINE_FUNC_OF_ARGS(1);
DEFINE_FUNC_OF_ARGS(2);
DEFINE_FUNC_OF_ARGS(3);
DEFINE_FUNC_OF_ARGS(4);
DEFINE_FUNC_OF_ARGS(5);
DEFINE_FUNC_OF_ARGS(6);
//...

}//end of namespace brpc
