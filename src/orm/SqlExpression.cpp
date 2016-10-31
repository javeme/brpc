#pragma once
#include "orm/Column.h"


namespace brpc{

int SQLExpression::debug(cstring frmt, ...)
{
	if(!DEBUG_SQL)
		return -1;

	va_list arg_ptr;
	va_start(arg_ptr, frmt);

	return vprintf(frmt, arg_ptr) + printf("\n");
}

}//end of namespace brpc