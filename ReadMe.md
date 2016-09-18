brpc lib
=====
a rpc lib for c++

---

## examples:

### example 1. hello service:

* 1.add class `MyRpcService` as a service provider:
```c++
class MyRpcService : public RpcService
{
public:
	MyRpcService(cstring name) : RpcService(name)
	{
		//register functions to provide service
		as(MyRpcService::hello);
		as(MyRpcService::sum);
	}
	
public:
	std::string hello(const char* who)
	{
		std::string str = "hello, ";
		str += who;
		return str;
	}
	
	double sum(int a, double b)
	{
		return a + b;
	}
};
```

* 2.add class `MyRpcApi` as stub:
```c++
	class MyRpcApi : public RpcApi
	{
	public:
		MyRpcApi(cstring url, cstring name, cstring password) : 
			checker(name, password),
			dispatcher("callback"),
			client(url, &dispatcher, &checker, "text/json", 1000*10),
			RpcApi("serviceName4Test")
		{
			ObjectList loginArgs;
			loginArgs.addValue(name);
			loginArgs.addValue(password);
			init(&client, loginArgs);
		}
		
		virtual ~MyRpcApi()
		{
			try{
				logout();
			}catch (Exception& e) {
				ErrorHandler::handle(e);
			}
		}
		
	public:
		std::string hello(const char* who)
		{
			ObjectList args;
			args.addValue(who);
			return call<std::string>("hello", args);
		}
		
		double sum(int a, double b)
		{
			ObjectList args;
			args.addValue(a);
			args.addValue(b);
			return call<double>("sum", args);
		}
	};
```

* 3.start server:
```c++
void mian()
{
	MyRpcService dispatcher("serviceName4Test");
	DefaultAuthChecker checker("test", "123456");

	RpcServer server("http://0.0.0.0", &checker, "text/json");

	printf("starting server...\n");
	server.start(&dispatcher);
	server.wait();
}
```

* 4.start client:
```c++
void mian()
{
	cstring url = "http://127.0.0.1";
	cstring name = "test", password = "123456";
	MyRpcApi myApi(url, name, password);

	std::string result = myApi.hello("brpc");
	printf("hello() returned: %s\n", result.c_str());

	double f = myApi.sum(2, 3.14);
	printf("sum() returned: %lf\n", f);
}
```

### example 2. test brpc with bat:

* 1.run server:

    execute `bin/brpc-server.bat`

* 2.run client:

    execute `bin/brpc-client*.bat`
	
	or
	
	execute brpc.exe with parameters, such as`brpc.exe client nova.sum 3 3.14`
	
	<br/>
	*all the client bats for test:*

		brpc-client echo.bat
		brpc-client echo - test n .bat
		brpc-client help.bat
		brpc-client ping.bat
		brpc-client sum.bat
		brpc-client play.bat
		brpc-client print.bat
		brpc-client exec output.bat
		brpc-client exec redirect
		brpc-client exec telnet.bat
		brpc-client exec telnet.bat
		brpc-client sub-sub-service-print.bat
		brpc-api-test.bat

