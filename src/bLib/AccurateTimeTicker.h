#pragma once

#include "Exception.h"

namespace bluemei{

#ifdef WIN32
#include <windows.h>
#include <winbase.h> //QueryPerformanceFrequency defined in this file
	
#define USECOND_PER_SECOND (1000*1000) //每秒的微秒数

class BLUEMEILIB_API AccurateTimeTicker : public Object
{
	LARGE_INTEGER m_begin,m_end;
	int m_nCountPerSec;//每秒的滴答次数
	float m_fUSecPerCount;//每滴答1次的微秒数
public:
	AccurateTimeTicker() {
		LARGE_INTEGER freqCounter;
		bool success=QueryPerformanceFrequency(&freqCounter)==TRUE;
		if(!success)
			throw Exception("AccurateTime::QueryPerformanceFrequency faild");
		m_nCountPerSec=(int)freqCounter.QuadPart;//为一秒钟的计时次数(大概2923105次)
		m_fUSecPerCount=(float)(USECOND_PER_SECOND*(1.0/m_nCountPerSec));//10000000*(每次的秒数)
	}
	void start() {
		m_end.QuadPart = 0;
		QueryPerformanceCounter(&m_begin);
	}
	//返回从开始到现在的时间间隔(单位:秒s)
	double getIntevalSecond() {
		QueryPerformanceCounter(&m_end);
		return (m_end.QuadPart-m_begin.QuadPart)/(double)m_nCountPerSec;
	}
	//返回从开始到现在的时间间隔(单位:微秒)
	unsigned long long getIntevalUSecond() {
		QueryPerformanceCounter(&m_end);
		return (unsigned long long)((m_end.QuadPart-m_begin.QuadPart)*m_fUSecPerCount);
	}
};

#else 
/* Linux */
#include <sys/time.h> // struct timeval
class AccurateTimeTicker : public Object
{
	struct timeval tmStart;
	struct timeval tmEnd;
public:
	AccurateTimeTicker() {
		;
	}
	void start() {
		gettimeofday(&tmStart,NULL);
	}
	//返回从开始到现在的时间间隔(单位:秒s)
	double getIntevalSecond() {
		gettimeofday(&tmEnd,NULL);
		return ((tmEnd.tv_usec-tmStart.tv_usec)/(double)USECOND_PER_SECOND)+(tmEnd.tv_sec-tmStart.tv_sec);
	}
	//返回从开始到现在的时间间隔(单位:微秒)
	unsigned long long getIntevalUSecond() {
		gettimeofday(&tmEnd,NULL);
		return (unsigned long long)((tmEnd.tv_usec-tmStart.tv_usec)+(tmEnd.tv_sec-tmStart.tv_sec)*USECOND_PER_SECOND);
	}
};

#endif



}//end of namespace bluemei