#pragma once
#include "stdafx.h"
#include "FunctionWrapper.h"

namespace bluemei{

//函数定义

// FUN_AND_PFUN_OF_ARGS(
// 	VA_ARGS(),
// 	VA_ARGS(),
// 	VA_ARGS(),
// 	VA_ARGS()
// 	)//多余出逗号

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

}//end of namespace bluemei
