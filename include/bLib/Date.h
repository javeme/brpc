#pragma once
#include "bluemeiLib.h"

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <sys/timeb.h>
#include <time.h>
#include "Object.h"
#include "BString.h"

namespace blib{

typedef tm DateInfo;

#define FROM_YEAR 1900
#define SECOND_MSECOND 1000
#define MINUTE_MSECOND (SECOND_MSECOND*60)
#define HOUR_MSECOND (MINUTE_MSECOND*60)
#define DAY_MSECOND (HOUR_MSECOND*24)
#define DATE_FORMAT_DEFAULT "%Y-%m-%d %H:%M:%S.%i"

class BLUEMEILIB_API Date : public Object
{
public:
	Date(void);
	Date(long long ms){
		operator=(ms);
	}
	~Date(void);
public:
	static Date getCurrentTime();
	static Date parseDate(const char* format,const char* strDate);
	static int month2Int(const char* month);
public:
	Date& operator=(long long ms);
	long operator-(const Date& other) const;
	Date operator+(long ms) const;
	bool operator<(const Date& other) const;

	virtual String toString() const;
	String formatDate( const char* format ) const;
	DateInfo getDateInfo() const;

	void setToCurrentTime();
	long long getTotalMillSecond() const;
public:
	void setYear(int i);
	void setMonth(int i);
	void setDay(int i);
	void setHour(int i);
	void setMinu(int i);
	void setSecond(int i);
	void setMillisecond(int i);
private:
	void initDate();
private:
	_timeb m_time;//用于存放时间
	//DateInfo m_dateInfo;//用于存放设置
};


#define COST_TIME(code_string, returnStr)\
{\
	Date timeStart=Date::getCurrentTime();\
	code_string;\
	Date timeEnd=Date::getCurrentTime();\
	int span=timeEnd-timeStart;\
	char _buf[128];\
	sprintf(_buf,"from %s to %s, cost %d ms",\
		timeStart.toString().c_str(),\
		timeEnd.toString().c_str(),\
		span);\
	returnStr=_buf;\
	_buf;/*用于gcc的返回*/\
}

#define COST_TIME_BOX(code_string, task)\
{\
	wstring wstr;\
	string _str;\
	COST_TIME(code_string,_str);\
	SocketTools::gbkToUnicode(wstr,_str.c_str());\
	::AfxMessageBox(CString(task)+_T(",")+wstr.c_str());\
}

}//end of namespace blib