#pragma once
//#include "stdafx.h"

#ifdef WIN32

// 从 Windows 头中排除极少使用的资料
#define WIN32_LEAN_AND_MEAN

// Windows 头文件:
#include <windows.h>


/************************************************************************/
/*                            屏蔽部分警告                               */
/************************************************************************/
//屏蔽忽略异常警告(http://www.myexception.cn/vc-mfc/165727.html)
#pragma warning(disable : 4290)

//屏蔽内部成员无导出警告
//(http://blog.csdn.net/sky101010ws/article/details/6780854)
#pragma warning(disable : 4251)


// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 BLUEMEILIB_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// BLUEMEILIB_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef BLUEMEILIB_EXPORTS
#define BLUEMEILIB_API __declspec(dllexport)
#define BLUEMEILIB_TEMPLATE BLUEMEILIB_API
#else
#define BLUEMEILIB_API __declspec(dllimport)
#define BLUEMEILIB_TEMPLATE
#endif // BLUEMEILIB_EXPORTS

#endif // WIN32


#include <xutility>
#include <iostream>
#include <string>
#include <vector>
#include <map>
//using namespace std;

#define List vector
#define Map map


namespace blib{

using std::vector;
using std::map;
using std::move;

#define CODE2STRING(code) _CODE2STRING(code)//可以替换普通代码和宏展开代码
#define _CODE2STRING(code) #code//可以替换普通代码和宏展开代码

}//end of namespace blib