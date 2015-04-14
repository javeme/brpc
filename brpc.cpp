// brpc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CheckMemLeaks.h"
CHECK_MEMORY_LEAKS

#include "SocketTools.h"
#include "P2pRpcServer.h"
#include "P2pRpcClient.h"
#include "DefaultAuthChecker.h"
#include "JsonSerializer.h"
#include "MyRpcService.h"
#include "ObjectMap.h"
#include "ObjectFactory.h"
#include "Log.h"

using namespace bluemei;

#include "MyRpcApi.h"
void testMyRpcApi()
{
	cstring url = "jmtp://127.0.0.1";
	cstring name = "test", password = "123456";
	MyRpcApi myApi(url, name, password);
		
	myApi.subscribe("nova.onclick", "callback.onclick");

	String sss = myApi.ping();

	myApi.print("test print!!!");

	TestObject* obj = new TestObject();
	obj->name = "Cat";
	obj->age = 28;
	obj->weight = 46.82f;
	obj->sex = true;
	myApi.print(obj);

	double f = myApi.sum(55, 33.8);
	bool sucess = myApi.playMusic("F:/歌曲/经典音乐/Beyond-光辉岁月.mp3");
	String result = myApi.execute("notepad test-notepad.txt");
	result = myApi.execute("ping baidu.com");
	
	MySubRpcApi* sub = (MySubRpcApi*)myApi.getSubService("sub");
	int len = sub->print("hello, sub rpc-api");

	getchar();
}

int run(int argc, char* argv[])
{
	Log* logger = Log::getLogger();
	cstring usage = "usage: brpc server|client [options...]\n";
	
	if (argc > 1)
	{
		String arg1 = argv[1];		
		if (arg1 == "server")
		{
			logger->info("brpc server starting...");

			MyRpcService dispatcher("nova");
			DefaultAuthChecker checker("test", "123456");
			P2pRpcServer server("http://0.0.0.0", &checker, "text/json");
			printf("server start...\n");
			server.run(&dispatcher);
		}
		else if(arg1 == "client")
		{
			logger->info("brpc client starting...");

			cstring url = "http://127.0.0.1";//http://192.168.1.106
			RpcService dispatcher;
			DefaultAuthChecker checker("", "");
			P2pRpcClient client(url, &dispatcher, &checker,"text/json");//"text/xml", "text/json", "application/brpc.bin"

			printf("client start...\n");
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
						printf("invalid para: %s\n", argv[i]);
				}

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
				logger->info(rs);
				delete result;

				msg = client.logout();
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
		else
			printf(usage);
	}
	else
		printf(usage);
	return 0;
}


#include "ClassField.h"


class TestField : public Object
{
public:
	DECLARE_DCLASS(TestField);
public:
	int aa;
	float bb;
	double cc;
	Double obj;
		
	Field<bool> sex;
	Field<int> age;
	Field<float> weight;
	FIELD(int, ival);
	FIELD(float, fval);
	FIELD(Object*, oval);
public:
	TestField(): oval(null) {}
	int ff();
};

REGISTER_CLASS(TestField);
REG_CLS_FIELD(TestField, bb);
REG_CLS_FIELD(TestField, cc);

void testField()
{
	REG_FIELD(TestField::obj);
	TestField test;
	test.oval = new TestField();
	auto aa = regField("aa", &TestField::aa);
	aa.set(test, 123);
	String sssaa=Value2String<const FieldInfo*>(&aa);

	regField("sex", &TestField::sex);
	auto field = regField("age", &TestField::age);
	test.sex = true;
	test.age = 28;
	int age = test.age + 2;
	field.set(test, 80);

	test.setAttributeT("bb", 3.14f);
	test.setAttributeT<Double>("obj", 3.14159260);
	test.setAttributeT<Field<int>>("age", (88));
	age = test.getAttributeT<Field<int>>("age");

	test.setAttribute("cc", &Double(6.28));

	String sss=Value2String<const FieldInfo*>(&field);
	auto cls = TestField::thisClass();
	auto flds = cls->allFields();
	std::cout<<flds.toString().c_str()<<std::endl;

	auto map = objectToMap(&test);
	//auto map = MapObjectHelper<TestField*, true>::object2map(&test);
	std::cout<<map->toString().c_str()<<std::endl;
	delete map;

	ObjectMap* map2 = new ObjectMap();
	map2->putValue("aa", 38);
	map2->put("age", new Field<int>(38));
	auto obj = valueOf<TestField*>(map2);
	delete obj;
	delete map2;

	valueOf<TestField*>(&sss);
}


int main(int argc, char* argv[])
{
	//main2(argc, argv);
	try{
		LogManager::instance().initLogger("brpc", "/var/log/brpc.log", Log::LOG_DEBUG);
	}catch (Exception& e){
		e.printStackTrace();
	}
	Log* log = Log::getLogger("brpc");
	log->info("brpc starting...");
		
	SocketTools::initSocketContext();
	try
	{
		log->trace("before running");
		//testField();
		run(argc, argv);
		log->trace("after running");
	}catch (Exception& e){
		e.printStackTrace();
		log->error(e.toString());
	}
	SocketTools::cleanUpSocketContext();

	log->info("brpc end");
	system("pause");
	return 0;
}
