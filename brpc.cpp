// brpc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CheckMemLeaks.h"
CHECK_MEMORY_LEAKS

#include "bLib/SimpleCfgFile.h"
#include "bLib/Log.h"
#include "bLib/SocketTools.h"

#include "MyRpcService.h"
#include "MyRpcApi.h"
#include "src/type/ObjectMap.h"
#include "src/rpc/server/RpcServer.h"

#ifndef WIN32
#include <signal.h>
#include <pthread.h>

class SignalHandler
{
public:
	static void catchSignals(int sigs[])
	{
		//signal(SIGSEGV, signalHandle);
		for(int i = 0; sigs[i] != 0; i++)
		{
			assert(SIG_ERR != signal(sigs[i], signalHandle));
		}
	}

	static void signalHandle(int sig)
	{
		printf("Signal %d\n", sig);
		Util::dumpStack();
		exit(-1);
	}

	static void ignoreSignal(int sig) throw(Exception)
	{
		struct sigaction sa;
		sa.sa_handler = SIG_IGN;
		sa.sa_flags = 0;
		if(sigemptyset(&sa.sa_mask) == -1 || sigaction(sig, &sa, 0) == -1)
		{
			throw Exception(String::format(
					"Failed to set ignore signal %d.", sig));
		}

		sigset_t signal_mask;
		sigemptyset(&signal_mask);
		sigaddset(&signal_mask, sig);
		if(pthread_sigmask(SIG_BLOCK, &signal_mask, NULL) != 0)
		{
			throw Exception(String::format(
					"Failed to set pthread ignore signal %d.", sig));
		}
	}

	static void ignoreSignals(int sigs[]) throw(Exception)
	{
		for(int i = 0; sigs[i] != 0; i++)
		{
			ignoreSignal(sigs[i]);
		}
	}
};

// for linux like
static void initSignalHandler() throw(Exception)
{
	int catches[] = {SIGSEGV, SIGBUS, SIGABRT, 0};
	SignalHandler::catchSignals(catches);

	int ignores[] = {SIGPIPE, 0};
	SignalHandler::ignoreSignals(ignores);
}

#else

static void initSignalHandler() throw(Exception)
{
}

#endif

using namespace brpc;

class LogErrorHandler : public blib::IErrorHandler
{
public:
	LogErrorHandler()
	{
		// set unexpected exception handler, for more details please see:
		// http://dev.yesky.com/307/2628307.shtml
		std::set_unexpected(LogErrorHandler::unexpectedException);
	}
public:
	virtual bool handle(const Exception& e)
	{
		Log* log = Log::getLogger();
		log->warn(e.toString());
		return true;
	}

	static void unexpectedException()
	{
		Log* log = Log::getLogger();
		const String PREFIX = "[Unexpected specified exception] ";

		try {
			throw;
		} catch (Exception& e) {
			printf("%s%s\n", PREFIX.c_str(), e.toString().c_str());e.printStackTrace();
			log->warn(PREFIX + e.toString());
			//rethrow
			throw RpcException(e);
		} catch (std::exception& e) {
			log->warn(PREFIX + e.what());
			//rethrow
			throw RpcException(e.what());
		} catch (...) {
			log->warn(PREFIX + "unknown exception");
			//rethrow
			throw;
		}
	}
};


String url = "http://127.0.0.1";//http://192.168.1.131
//String url = "jmtp://127.0.0.1";
//String url = "amqp://guest:guest@127.0.0.1:5672/"\
	"?self=node-1&destination=node-1&topic=rpc";

void testClient(const String& method, const ObjectList& args)
{
	RpcService dispatcher;
	DefaultAuthChecker checker("", "");
	//"text/xml", "text/json", "application/brpc.bin"
	RpcClient client(url, &dispatcher, &checker,"text/json", 1000*3);
	
	ObjectList loginArgs;
	loginArgs.addValue("test");
	loginArgs.addValue("123456");
	String msg = client.login(loginArgs);

	Object* result = client.call(method, args);
	String rs = String::format(">>>>> call method %s(%s): \n"
		"=====\n"
		"%s\n"
		"=====\n",
		method.toString().c_str(),
		args.toString().c_str(),
		result ? result->toString().c_str() : "<null>");
	printf(rs.c_str());

	delete result;
	msg = client.logout();
}

void testMyRpcApi()
{
	cstring name = "test", password = "123456";
	MyRpcApi myApi(url, name, password);

	myApi.subscribe("nova.onclick", "callback.onclick");

	String sss = myApi.ping();

	myApi.print("test print!!!");

	ScopePointer<TestObject> obj = new TestObject();
	obj->name = "Cat";
	obj->age = 28;
	obj->weight = 46.82f;
	obj->sex = true;
	myApi.print(obj);

	double f = myApi.sum(55, 33.8);
	bool sucess = myApi.playMusic("F:/歌曲/经典音乐/Beyond-光辉岁月.mp3");
	/*String result = myApi.execute("ping baidu.com");
	result = myApi.execute("notepad test-notepad.txt");*/

	MySubRpcApi* sub = (MySubRpcApi*)myApi.getSubService("sub");
	int len = sub->print("hello, sub rpc-api");

	getchar();
}

void testMyRpcApiPerf(int count, int threads=4)
{
	cstring name = "test", password = "123456";

	auto perfTest = [&, count](int t) {
		printf("start thread %d...\n", t);
		MyRpcApi myApi(url, name, password);

		String ping = myApi.ping();
		printf("ping: %s\n", ping.c_str());

		int timeouts = 0;
		Date startTime = Date::getCurrentTime();
		for (int i=0; i<count; i++)
		{
			try{
				String result = myApi.echo(String::format("fake-%d-%d", t, i));
				printf("echo() returned: %s\n", result.c_str());
				/*ObjectList args;
				args.addValue(String::format("fake-%d", i));
				myApi.cast("echo", args);*/
			}catch(TimeoutException& e){
				timeouts++;
			}catch(Exception& e){
				e.printStackTrace();
			}
		}
		Date endTime = Date::getCurrentTime();
		float seconds = (endTime - startTime) / 1000.0;
		float speed = count / seconds;
		printf("thread-%d finished, total %d(%d timeout) in "
				"%.2fs [%.2f reqs/s].\n", t, count, timeouts, seconds, speed);
	};

	for (int t=0; t<threads; t++) {
		Thread* thread = new LambdaThread([&, t]{
			try {
				perfTest(t);
			} catch (Exception& e) {
				printf("Thread %d finished with exception.\n", t);
				e.printStackTrace();
			}
		});
		thread->start();
	}
	getchar();
}

int run(int argc, char* argv[]) throw(Exception)
{
	Log* logger = Log::getLogger();
	cstring usage = "usage: brpc server|client [options...]\n";

	try {
		SimpleCfgFile cfg("brpc.ini");
		url = cfg["default"]["url"];
		//url = cfg["default"].option<String>("url"/*, url*/);
	} catch(Exception& e) {
		logger->warn("exception when reading conf file: " + e.toString());
	}

	if (argc > 1)
	{
		String arg1 = argv[1];
		if (arg1 == "server")
		{
			printf("brpc server starting...\n");
			logger->info("brpc server starting...");

			MyRpcService dispatcher("nova");
			DefaultAuthChecker checker("test", "123456");
			cstring url = "http://0.0.0.0";
			//String url = "amqp://guest:guest@127.0.0.1:5672/?self=node-1";
			RpcServer server(url, &checker, "text/json");
			printf("server start...\n");
			server.start(&dispatcher);
			server.wait();
		}
		else if(arg1 == "client")
		{
			logger->info("brpc client starting...");
			
			//such as: echo "hello rpc"
			if (argc > 2)
			{
				String method = argv[2];
				ObjectList args;
				for(int i=3; i<argc; i++){
					String arg = argv[i];
					char t = arg.charAt(0);
					if (t == '"')
						args.addValue(arg.substring(1, arg.length()-2).c_str());
					else if (t == '\'')
						args.addValue(arg.substring(1, arg.length()-2).c_str());
					else if (isdigit(t)){
						if (arg.find(".") > 0)
							args.addValue(CodeUtil::str2Float(arg.c_str()));
						else
							args.addValue(CodeUtil::str2Int(arg.c_str()));
					}
					else if (t == 't' && arg == "true")
						args.addValue(true);
					else if (t == 'f' && arg == "false")
						args.addValue(false);
					else
						printf("invalid parameter: %s\n", argv[i]);
				}
				testClient(method, args);
			}
			else
			{
				printf(usage);
				printf("please input a method name.\n");
			}
		}
		else if(arg1 == "api-test")
		{
			logger->info("brpc api-test starting...");
			testMyRpcApi();
		}
		else if(arg1 == "api-test-perf")
		{
			String count = "1000";
			if (argc > 2)
				count = argv[2];
			testMyRpcApiPerf(CodeUtil::str2Int(count));
		}
		else
			printf(usage);
	}
	else
		printf(usage);
	return 0;
}


int main(int argc, char* argv[])
{
	//_CrtSetBreakAlloc(3011);
	initSignalHandler();

	System::instance().init();
	BRpcUtil::setBrpcDebug(false);

	String name = "brpc";
	if (argc > 1){
		name += "-";
		name += argv[1];
	}

	try{
		String file = String::format("/var/log/brpc/%s.log", name.c_str());
		LogManager::instance().initLogger(name, file, Log::LOG_DEBUG);
	}catch (Exception& e){
		e.printStackTrace();
	}

	Log* log = Log::getLogger(name);
	log->info("brpc starting...");
	log->updateFormatter("$time [Thread-$thread] [$level] $message");

	LogErrorHandler logHandler;
	ErrorHandler::setHandler(&logHandler);

	SocketTools::initSocketContext();

	try
	{
		log->trace("before running");
		run(argc, argv);
		log->trace("after running");
	}catch (Exception& e){
		e.printStackTrace();
		log->error("exit with exception: " + e.toString());
	}

	SocketTools::cleanUpSocketContext();

	log->info("brpc end");
	System::instance().destroy();

	system("pause");
	return 0;
}
