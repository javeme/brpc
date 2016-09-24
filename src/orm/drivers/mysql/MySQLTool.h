#pragma once
#include <shellapi.h>
#include <time.h>
#include "orm/drivers/mysql/MySQLConnection.h"

#define DEFAULT_TIME_FORMAT "%Y-%m-%d %H:%M:%S"


namespace brpc{

class MySQLTool : public Object
{
public:
	static string getCurrentDir()
	{
		char buf[MAX_PATH];
		memset(buf,0,MAX_PATH);
		GetCurrentDirectoryA(MAX_PATH,buf);
		return buf;
	}

	static string getCurrentTime(cstring format=DEFAULT_TIME_FORMAT)
	{
		time_t t = time(0);
		char buf[64];
		memset(buf,0,sizeof(buf));
		strftime(buf, sizeof(buf), format, localtime(&t));
		return buf;
	}

	static bool startMySQL()
	{
		//"start bin/mysqld-nt.exe --no-defaults --skip-bdb --skip-innodb --log=log.txt"
		string path=getCurrentDir();
		path+="/mysql5.0/bin/mysqld-nt.exe";//D:/c++/mfc/DiagClientMdi/MySQLConnector
		cstring parameters=" --no-defaults --skip-bdb --skip-innodb --log=log.txt";
		HINSTANCE h=ShellExecuteA(NULL, "open", path.c_str(), parameters, NULL, SW_HIDE);//SW_HIDE
		if((int)h<=32)
		{
			/*
			0 操作系统内存溢出或者系统资源不够
			ERROR_FILE_NOT_FOUND 指定的文件没有找到
			ERROR_PATH_NOT_FOUND 指定的地址没有找到
			ERROR_BAD_FORMAT EXE文件是一个无效的PE文件格式，或者EXE文件损坏了
			SE_ERR_ACCESSDENIED 无权访问该文件
			SE_ERR_ASSOCINCOMPLETE 文件关联无效
			SE_ERR_DDEBUSY DDE事物无法完成相应，因为DDE事物正在被处理
			SE_ERR_DDEFAIL DDE事务失败。
			SE_ERR_DDETIMEOUT DDE事务无法完成响应，因为请求超时
			SE_ERR_DLLNOTFOUND 指定的DLL文件没找到
			SE_ERR_FNF 指定文件未找到
			SE_ERR_NOASSOC 没有关联程序
			SE_ERR_OOM 内存不够，无法完成响应


			SE_ERR_PNF 指定的地址未找到

			SE_ERR_SHARE 共享越界异常*/
			printf("ShellExecute error:%d\n",(int)h);
			return false;
		}
		WaitForSingleObject(h,1000*1);
		return true;
	}

	static bool stopMySQL(const string& passw)
	{
		/*string path=getCurrentDir();
		path+="/mysql5.0/mysql_stop.bat";
		HINSTANCE h=ShellExecuteA(NULL,  "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);//SW_HIDE
		WaitForSingleObject(h,1000*30);*/

		//bin\mysqladmin --user=root --password= shutdown
		string path=getCurrentDir();
		path+="/mysql5.0/bin/mysqladmin.exe";
		string parameters="  --user=root --password="+passw+" shutdown";
		HINSTANCE h=ShellExecuteA(NULL, "open", path.c_str(), parameters.c_str(), NULL, SW_HIDE);
		return (int)h>32;
	}

	static void printResult(ResultSet* rs)
	{
		printf("--------------------------------------------------------\n");
		int fieldNum=rs->fieldsCount();
		while(rs->next())
		{
			for (int i=0; i<fieldNum; i++) {
				if(i>0)
					printf(" | ");
				printf("%s",rs->getString(i));
			}
			printf("\n--------------------------------------------------------\n");
		}
		rs->beforeFirst();

		/*rs->afterLast();
		while(rs->previous())
		{
			for (int i=0; i<fieldNum; i++) {
				if(i>0)
					printf(" | ");
				printf("%s",rs->getString(i));
			}
			printf("\n--------------------------------------------------------\n");
		}
		rs->afterLast();*/
	}
};


}//end of namespace brpc