// brpc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "FuncDispatcher.h"
#include "ObjectMap.h"
using namespace bluemei;

#include "CheckMemLeaks.h"
#include "JsonSerializer.h"
CHECK_MEMORY_LEAKS

double add(int a, double b)
{
	return a+b;
}

double mul(int a, double b)
{
	return a*b;
}

int testStr(int a, cstring b)
{
	return a+strlen(b);
}

int testZero(void)
{
	return 0;
}

float testOne(float a)
{
	return a;
}

int testPrint(cstring str)
{
	return printf(str);
}

void testVoid()
{
	printf("void function\n");
}

cstring echo(cstring str)
{
	printf("echo: %s \n", str);
	return str;
}

lint testSix(bool a1, char a2, short a3, cstring a4, lint a5, unsigned int a6)
{
	return a1 + a2 + a3 + strlen(a4) + a5 + a6;
}

std::vector<int> testIntList(int a, int b)
{
	std::vector<int> list;
	list.push_back(a);
	list.push_back(b);
	return list;
}

std::vector<std::string> testStringList(cstring str)
{
	std::vector<std::string> list;
	String str2(str);
	auto l = str2.splitWith(",");
	
	for(int i=0; i<l.size(); i++)
		list.push_back(l[i]);
	return list;
}

std::string testList2String(std::vector<int> list)
{
	String str;

	for(int i=0; i<list.size(); i++)
		str += String::format("%d,", list[i]);
	return str;
}

std::map<std::string, int> testString2Map(cstring str)
{
	std::map<std::string, int> map;

	String str2(str);
	auto list = str2.splitWith(",");
	for(int i=0; i<list.size(); i++)
	{
		auto kv= list[i].splitWith(":");
		if(kv.size() == 2)
			map[kv[0]] = BRpcUtil::str2Int(kv[1]);
	}

	return map;
}

std::string testMap2String(std::map<std::string, double> map)
{
	StringBuilder sb(map.size() * 20);
	for (auto itor = map.begin(); itor != map.end(); ++itor)
	{
		const std::string& name = itor->first;
		double val = itor->second;

		sb.append(name.c_str()).append(':');
		sb.append(String::format("%lf",val)).append(',');
	}
	if (sb.length() > 0 && sb.charAt(sb.length()-1) == ',')
	{
		sb.deleteCharAt(sb.length()-1);
	}

	return sb.toString().c_str();
}

class User
{
public:
	std::string name;
	int age;
};

template <>
struct Converter<User*>
{
	static inline User* valueOf(Object* obj)
	{
		checkNullPtr(obj);
		ObjectMap* objMap = castAndCheck<ObjectMap*>(obj, "map");

		User* user = new User();//如何释放?????????
		//user->name = objMap["name"]->toString().c_str();
		objMap->getValue("name", user->name);
		objMap->getValue("age", user->age);
		return user;
	}

	static inline Object* toObject(const User* user)
	{
		checkNullPtr(user);
		ObjectMap* objMap = new ObjectMap();
		objMap->putValue("name", user->name);
		objMap->putValue("age", user->age);
		//delete user;
		return objMap;
	}
};

User* testReturnUser(cstring name, int age)
{
	User *user = new User();//如何释放?????????
	user->name = name;
	user->age = age;
	return user;
}

std::string testGetUserName(User *user)
{
	return user->name;
}

namespace testns{
class TestNS : public Object
{
public:
	int testNS() {
		return 60;
	}
};
}

class TestClass : public Object
{
public:
	TestClass(int age=0):m_age(age){}

	virtual int increase()
	{
		return ++m_age;
	}

	virtual void showAge(cstring str)
	{
		printf("%s %d\n",str, m_age);
	}

	int getAge() const { return m_age; }
	void setAge(int val) { m_age = val; }
protected:
	int m_age;
};

template <>
struct Converter<TestClass*>
{
	static inline TestClass* valueOf(Object* obj)
	{
		checkNullPtr(obj);
		TestClass* testClass = dynamic_cast<TestClass*>(obj);
		if(testClass)
			return testClass;

		ObjectMap* objMap = castAndCheck<ObjectMap*>(obj, "map");

		testClass = new TestClass();
		int age;
		objMap->getValue("age", age);
		testClass->setAge(age);
		return testClass;
	}

	static inline Object* toObject(const TestClass* testClass)
	{
		ObjectMap* objMap = new ObjectMap();
		objMap->putValue("age", testClass->getAge());
		return objMap;
	}
};


void testBaseType(FuncDispatcher& dispatcher) 
{
	dispatcher.registerFunction("add", add);
	dispatcher.registerFunction("add", add);
	dispatcher.registerFunction<double(int,double)>("add", add);

	dispatcher.regFunc(mul);
	dispatcher.regFunc(testStr);

	dispatcher.regFunc(testZero);
	dispatcher.regFunc(testOne);
	dispatcher.regFunc(testPrint);

	dispatcher.regFunc(testVoid);
	dispatcher.regFunc(testSix);

	//dispatcher.unregistetFunction("add", add);

	try{
		ObjectList args;
		args.add(toObject(12));
		args.add(toObject(6));

		Object* result = dispatcher.call("add", args);
		delete result;

		result = dispatcher.call("mul", args);
		delete result;

	}catch (Exception& e){
		e.printStackTrace();
	}

	try{
		ObjectList args;
		//args.addValue(3.14);

		Object* result = dispatcher.call("testZero", args);
		delete result;

		result = dispatcher.call("testVoid", args);
		delete result;

		args.addValue(1);
		result = dispatcher.call("testOne", args);
		delete result;

		ObjectList argsPrint;
		argsPrint.addValue("hello, rpc.\n");
		result = dispatcher.call("testPrint", argsPrint);
		delete result;
	}catch (Exception& e){
		e.printStackTrace();
	}

	try{
		ObjectList args;
		args.addValue(true);
		args.addValue('h');
		args.addValue(short(60));
		args.addValue("arg4");
		args.addValue(3.14);
		args.addValue(/*unsigned */int(6));

		Object* result = dispatcher.call("testSix", args);
		delete result;
	}catch (Exception& e){
		e.printStackTrace();
	}
}

void testListType(FuncDispatcher& dispatcher) 
{
	dispatcher.regFunc(testIntList);
	dispatcher.regFunc(testStringList);
	dispatcher.regFunc(testList2String);	

	try{
		ObjectList args;
		args.addValue(2);
		args.addValue(4);
		Object* result = dispatcher.call("testIntList", args);
		delete result;		
	}catch (Exception& e){
		e.printStackTrace();
	}

	try{
		ObjectList args;
		args.addValue("123,hello,blue,china");
		Object* result = dispatcher.call("testStringList", args);
		delete result;		
	}catch (Exception& e){
		e.printStackTrace();
	}

	try{
		ObjectList args;
		ObjectList *arg1 = new ObjectList();
		arg1->addValue(2);
		arg1->addValue(4);
		arg1->addValue(6);
		args.add(arg1);
		Object* result = dispatcher.call("testList2String", args);
		delete result;
	}catch (Exception& e){
		e.printStackTrace();
	}
}

void testMapType(FuncDispatcher& dispatcher) 
{
	dispatcher.regFunc(testString2Map);
	dispatcher.regFunc(testMap2String);	

	try{
		ObjectList args;
		args.addValue("hj:27,bluemei:25,pp:28");
		Object* result = dispatcher.call("testString2Map", args);
		delete result;		
	}catch (Exception& e){
		e.printStackTrace();
	}

	try{
		ObjectList args;
		ObjectMap *arg1 = new ObjectMap();
		arg1->putValue("first",2.5);
		arg1->putValue("second",3.14);
		arg1->putValue("third",6.8);
		args.add(arg1);
		Object* result = dispatcher.call("testMap2String", args);
		delete result;
	}catch (Exception& e){
		e.printStackTrace();
	}
}

void testObjectType(FuncDispatcher& dispatcher)
{
	dispatcher.regFunc(testReturnUser);
	dispatcher.regFunc(testGetUserName);	

	try{
		ObjectList args;
		args.addValue("hj");
		args.addValue(27);
		Object* result = dispatcher.call("testReturnUser", args);
		delete result;		
	}catch (Exception& e){
		e.printStackTrace();
	}

	try{
		ObjectList args;
		ObjectMap *arg1 = new ObjectMap();
		arg1->putValue("name","blue");
		arg1->putValue("age",25);
		args.add(arg1);
		Object* result = dispatcher.call("testGetUserName", args);
		delete result;
	}catch (Exception& e){
		e.printStackTrace();
	}
}

void testNamespace(FuncDispatcher& dispatcher) 
{
	testns::TestNS *tester2 = new testns::TestNS();
	dispatcher.regVar(tester2);
	
	auto testNS = &testns::TestNS::testNS;
	dispatcher.regFunc(testNS);
	dispatcher.regFunc(&testns::TestNS::testNS);

	try{
		ObjectList args;
		args.add(new ObjectRef("tester2", &dispatcher));//this

		auto result = dispatcher.call("testns.TestNS.testNS", args);
		delete result;
	}catch (Exception& e){
		e.printStackTrace();
	}	
}

void testClassFunc(FuncDispatcher& dispatcher) 
{
	TestClass *tester = new TestClass(18);
	dispatcher.regVar(tester);

	auto increase = &TestClass::increase;
	dispatcher.regFunc(increase);

	auto showAge = &TestClass::showAge;
	dispatcher.regFunc(showAge);

	auto getAge = &TestClass::getAge;
	//dispatcher.regFunc(getAge);//error C2440: “初始化”: 无法从“int (__thiscall TestClass::* )(void) const”转换为“int”	

	try{
		ObjectList args;
		args.add(new ObjectRef("tester", &dispatcher));//this

		args.addValue("Now, my age is:");
		Object* result = dispatcher.call("showAge", args);
		delete result;
		args.remove(1);

		result = dispatcher.call("increase", args);
		delete result;

		args.addValue("After a year, my age is:");
		result = dispatcher.call("showAge", args);
		delete result;
	}catch (Exception& e){
		e.printStackTrace();
	}
}

void testDispatcher()
{
	FuncDispatcher dispatcher;
	
	printf("testBaseType======================\n");
	testBaseType(dispatcher);

	printf("testListType======================\n");
	testListType(dispatcher);

	printf("testMapType=======================\n");
	testMapType(dispatcher);

	printf("testObjectType====================\n");
	testObjectType(dispatcher);

	printf("testNamespace=====================\n");
	testNamespace(dispatcher);
	
	printf("testClassFunc=====================\n");
	testClassFunc(dispatcher);
}

void testJson()
{
	printf("testJson=====================\n");
	ObjectList args;

	ObjectMap *arg1 = new ObjectMap();
	arg1->putValue("name","blue");
	arg1->putValue("age",25);
	args.add(arg1);

	ObjectList arg2;
	arg2.add(arg1);
	arg2.add(arg1);
	args.add(&arg2);

	args.addValue(3.14);
	
	ObjectMap *struc = new ObjectMap();
	struc->put("list1",&args);
	struc->put("list2",&args);

	Type2JsonSerializer v;
	String json = v.toJson(struc);
	printf("%s\n", json.c_str());
}

int main2(int argc, char* argv[])
{
	//_CrtSetBreakAlloc(1779);
	cstring ssssss=CODE2STRING(CLS_PF_OF_ARGS(0));

	testDispatcher();
	testJson();
	
	system("pause");
	return 0;
}
