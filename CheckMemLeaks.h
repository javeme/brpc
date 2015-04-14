#pragma once
#include "stdafx.h"
#include <assert.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define checkMemLeaks 1
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#define new DEBUG_CLIENTBLOCK
#else
#define checkMemLeaks 0
#define DEBUG_CLIENTBLOCK
#endif
#define _CRTDBG_MAP_ALLOC

#define CHECK_MEMORY_LEAKS static CheckMemLeaksRegister rCheckMemLeaks;

struct CheckMemLeaksRegister
{
	static void exitAndCheck(){
		int i = _CrtDumpMemoryLeaks();
		//assert( i == 0);
	}
	CheckMemLeaksRegister()
	{
		//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		//if(checkMemLeaks)
		//	atexit(exitAndCheck);
	}
	~CheckMemLeaksRegister()
	{
		exitAndCheck();
	}
};