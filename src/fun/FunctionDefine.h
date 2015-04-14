#pragma once
#include "stdafx.h"
#include "FunctionWrapper.h"
#include "mfor.h"

namespace bluemei{

//函数定义

// FUN_AND_PFUN_OF_ARGS(
// 	VA_ARGS(),
// 	VA_ARGS(),
// 	VA_ARGS(),
// 	VA_ARGS()
// 	)//多余出逗号

/*
FUN_AND_PFUN_OF_ARGS(
	VA_ARGS(TYPENAME_ARG(Arg1)),
	VA_ARGS(		 ARG(Arg1)),
	VA_ARGS(	ARG_TYPE(Arg1)),
	VA_ARGS(VALUE_OF_ARG(Arg1))
	)

FUN_AND_PFUN_OF_ARGS(
	VA_ARGS(TYPENAME_ARG(Arg1), TYPENAME_ARG(Arg2)),
	VA_ARGS(		 ARG(Arg1), 		 ARG(Arg2)),
	VA_ARGS(	ARG_TYPE(Arg1); 	ARG_TYPE(Arg2)),
	VA_ARGS(VALUE_OF_ARG(Arg1), VALUE_OF_ARG(Arg2))
	)
*/

/*
//#define FORA0(f)    f(0,)
//#define FORA1(f)    f(1,FORA0(f))
#define FORA0(f)    
#define FORA1(f)    f##1
#define FORA2(f)    f(2,FORA1(f))
#define FORA3(f)    f(3,FORA2(f))
#define FORA4(f)    f(4,FORA3(f))
*/

#define _TYPENAME_ARGS1 ,typename Arg1
#define _TYPENAME_ARGS(n, m)  m, typename JOIN(Arg, n)

#define _ARGS1 Arg1
#define _ARGS(n, m)  m, JOIN(Arg, n)

#define _INDEX_ARGS1 Arg1,
#define _INDEX_ARGS(n, m)  m JOIN(Arg, n),	

#define _ARG_TYPES1 ARG_TYPE(Arg1)
#define _ARG_TYPES(n, m)  m; ARG_TYPE(JOIN(Arg, n))

#define _VALUE_OF_ARGS1 VALUE_OF_ARG(Arg1)
#define _VALUE_OF_ARGS(n, m)  m, VALUE_OF_ARG(JOIN(Arg, n))

#define	_FUN_R_ARGS(R, ARGS) R(ARGS)
#define	_PFN_R_ARGS(R, ARGS) R(*)(ARGS)
#define _CPFRARGS(C, R, ARGS) R(C::*)(ARGS)

#define FUNC_OF_ARGS(n) 								 							 \
	FUN_OF_ARGS(											 						 \
		VA_ARGS(										  R),						 \
		VA_ARGS(			  JOIN(FORA, n)( _TYPENAME_ARGS)),						 \
		VA_ARGS(_FUN_R_ARGS(R,JOIN(FORA, n)(		  _ARGS))),						 \
		VA_ARGS(			  JOIN(FORA, n)(		  _ARGS)),						 \
		VA_ARGS(			  JOIN(FORA, n)(	_INDEX_ARGS)),						 \
		VA_ARGS(			  JOIN(FORA, n)(	 _ARG_TYPES)),						 \
		VA_ARGS(			  JOIN(FORA, n)( _VALUE_OF_ARGS))						 \
		)														 

#define PFUNC_OF_ARGS(n) 								 							 \
	FUN_OF_ARGS(											 						 \
		VA_ARGS(										  R),						 \
		VA_ARGS(			  JOIN(FORA, n)( _TYPENAME_ARGS)),						 \
		VA_ARGS(_PFN_R_ARGS(R,JOIN(FORA, n)(		  _ARGS))),						 \
		VA_ARGS(			  JOIN(FORA, n)(		  _ARGS)),						 \
		VA_ARGS(			  JOIN(FORA, n)(	_INDEX_ARGS)),						 \
		VA_ARGS(			  JOIN(FORA, n)(	 _ARG_TYPES)),						 \
		VA_ARGS(			  JOIN(FORA, n)( _VALUE_OF_ARGS))						 \
		)	

#define CLS_PF_OF_ARGS(n) 								 							 \
	CLS_FUN_OF_ARGS(																 \
		VA_ARGS(										  C),						 \
		VA_ARGS(										  R),						 \
		VA_ARGS(			  JOIN(FORA, n)( _TYPENAME_ARGS)),						 \
		VA_ARGS(_CPFRARGS(C,R,JOIN(FORA, n)(		  _ARGS))),						 \
		VA_ARGS(			  JOIN(FORA, n)(		  _ARGS)),						 \
		VA_ARGS(			C,JOIN(FORA, n)(	_INDEX_ARGS)),						 \
		VA_ARGS(			  JOIN(FORA, n)(	 _ARG_TYPES)),						 \
		VA_ARGS(			  JOIN(FORA, n)( _VALUE_OF_ARGS)),						 \
		VA_ARGS(										   )						 \
		)

#define CONST_CLS_PF_OF_ARGS(n) 								 					 \
	CLS_FUN_OF_ARGS(																 \
		VA_ARGS(										  C),						 \
		VA_ARGS(										  R),						 \
		VA_ARGS(			  JOIN(FORA, n)( _TYPENAME_ARGS)),						 \
		VA_ARGS(_CPFRARGS(C,R,JOIN(FORA, n)(		  _ARGS))),						 \
		VA_ARGS(			  JOIN(FORA, n)(		  _ARGS)),						 \
		VA_ARGS(			C,JOIN(FORA, n)(	_INDEX_ARGS)),						 \
		VA_ARGS(			  JOIN(FORA, n)(	 _ARG_TYPES)),						 \
		VA_ARGS(			  JOIN(FORA, n)( _VALUE_OF_ARGS)),						 \
		VA_ARGS(									  const)						 \
		)

#define DEFINE_FUNC_OF_ARGS(n)														 \
	FUNC_OF_ARGS(n)																	 \
	PFUNC_OF_ARGS(n)																 \
	CLS_PF_OF_ARGS(n)																 \
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

}//end of namespace bluemei
