#pragma once
#include "stdafx.h"
#include "src\rpc\server\RpcService.h"
#include <shellapi.h>
#include "LambdaThread.h"
using namespace brpc;

class TestObject : public Object
{
public:
	DECLARE_DCLASS(TestObject);
public:
	FIELD(String, name);
	FIELD(int, age);
	FIELD(double, weight);
	FIELD(bool, sex);
};

class AgeManager : public Object
{
public:
	AgeManager():age(0){}

public:
	int getAge() const { return age; }
	void setAge(int val) { age = val; }
	void addAge() { age++; }
private:
	int age;
};

class MyOtherRpcService : public RpcService
{
public:
	MyOtherRpcService(cstring name) : RpcService(name)
	{
		as(MyOtherRpcService::print);
	}
	virtual ~MyOtherRpcService()
	{
	}
public:
	int print(cstring str)
	{
		return printf("other service print: %s\n", str);
	}
};

class MySub2RpcService : public RpcService
{
public:
	MySub2RpcService(cstring name) : RpcService(name)
	{
		as(MySub2RpcService::print);
	}
	virtual ~MySub2RpcService()
	{
	}
public:
	int print(cstring str)
	{
		return printf("sub2 service print: %s\n", str);
	}
};

class MySubRpcService : public RpcService
{
public:
	MySubRpcService(cstring name) : RpcService(name), sub2Service("sub2")
	{
		as(MySubRpcService::print);
		this->addSubSevice(&sub2Service);
	}
	virtual ~MySubRpcService()
	{
	}
public:
	int print(cstring str)
	{
		return printf("sub service print: %s\n", str);
	}
private:
	MySub2RpcService sub2Service;
};

class MyRpcService : public RpcService
{
public:
	MyRpcService(cstring name) : RpcService(name), 
		otherService("other"), subService("sub")
	{
		this->currentDir = "";

		as(MyRpcService::sum);
		as(MyRpcService::playMusic);
		as(MyRpcService::setCurrentDir);
		as(MyRpcService::getCurrentDir);
		as(MyRpcService::listCurrentDir);
		as(MyRpcService::execute);
		as(MyRpcService::executeNoOutput);

		int(MyRpcService::*prints)(cstring) = &MyRpcService::print;
		asnf("print", prints);
		int(MyRpcService::*printi)(int) = &MyRpcService::print;
		asnf("print", printi);
		int(MyRpcService::*printf)(double) = &MyRpcService::print;
		asnf("print", printf);
		int(MyRpcService::*printb)(bool) = &MyRpcService::print;
		asnf("print", printb);
		int(MyRpcService::*printo)(TestObject*) = &MyRpcService::print;
		asnf("print", printo);

		amName = String::format("%s.%s", name, "am");
		(void)regObject(amName, &am);
		as(AgeManager::getAge);
		as(AgeManager::setAge);
		as(AgeManager::addAge);

		this->addSubSevice(&subService);

		this->extendService(&otherService);
	}
	virtual ~MyRpcService()
	{
		(void)unregObject(amName);

		this->excludeService(&otherService);
	}
public:
	double sum(int a, double b)
	{
		return a+b;
	}
	bool playMusic(cstring name)
	{
		Thread* trd = new LambdaThread([&](){ 
			int counter = 0;
			while(counter < 20){
				onEventClick(counter++);
				LambdaThread::sleep(1000*5);
			}
		}, null);
		trd->start();

		String path = name;
		if(currentDir != "")
			path = currentDir + "/" + name;
		return this->open("wmplayer", path);
	}
	String getCurrentDir() const { 
		return currentDir; 
	}
	void setCurrentDir(cstring val) { 
		currentDir = val; 
	}
	String listCurrentDir() {		
		return execute("dir \"" + currentDir + "\"");
	}
	bool executeNoOutput(cstring cmd)
	{
		return this->exec(cmd);
	}
	String execute(cstring cmd)
	{
		return this->executeCommand(cmd);
	}
	virtual String echo(cstring str)
	{
		return String("MyRpcService: ") + str;
	}

	int print(cstring str)
	{
		return printf("str: %s\n", str);
	}
	int print(String str)
	{
		return print(str.c_str());
	}
	int print(int v)
	{
		return printf("int: %d\n", v);
	}
	int print(double v)
	{
		return printf("double: %lf\n", v);
	}
	int print(bool v)
	{
		return printf("bool: %s\n", v?"true":"false");
	}
	int print(TestObject* obj)
	{
		print("TestObject {");
		print(obj->name);
		print(obj->sex);
		print(obj->age);
		print(obj->weight);
		print("}");
		return 0;
	}
	
	void onEventClick(int count)
	{
		ObjectList args;
		args.addValue(count);
		publishEvent("onclick", args);
	}
protected:
	AgeManager am;
	String amName;

	MyOtherRpcService otherService;

	MySubRpcService subService;
private:
	String currentDir;//path for play music
private:
	static bool open(cstring name, cstring para)
	{
		String str = String::format("\"%s\"", name);
		name = str.c_str();
		HINSTANCE i = ::ShellExecuteA(NULL, "open",
			name, para, "", SW_HIDE);
		return int(i)>32;
	}
	//÷¥––cmd√¸¡Ó
	static bool exec(cstring cmd)
	{
		String str = String::format("/c \"%s\"", cmd);
		cmd = str.c_str();
		HINSTANCE i = ::ShellExecuteA(NULL, "open",
			"cmd.exe", cmd, "", SW_HIDE);//SW_SHOWNORMAL
		return int(i)>32;
	}
	//÷¥––cmd√¸¡Ó,≤¢ªÿœ‘
	static String executeCommand(cstring cmd)
	{
		String str = String::format("cmd /c \"%s\"", cmd);
		cstring cmdLine = str.c_str();

		HANDLE hRead = 0;
		HANDLE hWrite = 0;

		SECURITY_ATTRIBUTES sas;
		sas.bInheritHandle=TRUE;
		sas.lpSecurityDescriptor=NULL;
		sas.nLength=sizeof(SECURITY_ATTRIBUTES);

		CreatePipe(&hRead,&hWrite,&sas,sas.nLength);

		STARTUPINFOA si;
		ZeroMemory(&si,sizeof(STARTUPINFO));

		GetStartupInfoA(&si);
		//si.hStdInput=hRead2;
		si.hStdOutput=hWrite;
		si.hStdError=hWrite;
		si.wShowWindow=SW_SHOWNORMAL;
		si.dwFlags=STARTF_USESTDHANDLES;
		
		PROCESS_INFORMATION pi;

		BOOL hProcess = CreateProcessA(NULL,(char*)cmdLine,&sas,NULL,TRUE,0,NULL,NULL,&si,&pi);
		if(!hProcess) 
		{ 
			CloseHandle(hRead); 
			CloseHandle(hWrite); 
			hRead = NULL; 
			hWrite = NULL;
			return String::format("execute cmd \"%s\" failed", cmd);
		}

		CloseHandle(hWrite);

		const static int READ_BUF_SIZE = 1024;
		char buffer[READ_BUF_SIZE];
		::memset(buffer,0,READ_BUF_SIZE);

		string strOut = cmd;
		strOut.append("\n");
		DWORD bytesRead = 0;
		while (true)
		{
			if (ReadFile(hRead,buffer,READ_BUF_SIZE,&bytesRead,NULL))
				strOut.append(buffer,bytesRead);
			else{
				DWORD result = WaitForSingleObject(pi.hProcess,0);//INFINITE
				if(WAIT_TIMEOUT == result)
					continue;
				else if(WAIT_OBJECT_0 == result){
					DWORD dwExitCode = 0;
					(void)GetExitCodeProcess(pi.hProcess, &dwExitCode);
					strOut.append(String::format("<exit code %d>", dwExitCode).c_str());				
					break;
				}
				else if(WAIT_FAILED == result){
					strOut.append("<wait for excuting cmd failed>");
					break;
				}
				else{
					strOut.append("<error>");
					break;
				}
			}
		}
		CloseHandle(hRead);

		/*CodeUtil::replaceString(strOut,"\r","");
		CodeUtil::replaceString(strOut,"\n","");
		CodeUtil::replaceString(strOut,"/","//");*/

		//printf(strOut.c_str());
		return strOut;
	}
};