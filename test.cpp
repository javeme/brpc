// brpc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "FuncDispatcher.h"
#include "ObjectMap.h"
#include "JsonSerializer.h"


using namespace bluemei;
using namespace brpc;


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

lint testSix(bool a1, char a2, short a3, cstring a4, double a5, int a6)
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

static int g_userCount = 0;

class User
{
public:
	std::string name;
	int age;

	User()
	{
		g_userCount++;
		printf("User count=%d\n", g_userCount);
	}
	User(const User& other)
	{
		g_userCount++;
		printf("User& count=%d\n", g_userCount);
		*this = other;
	}
	virtual ~User()
	{
		g_userCount--;
		printf("~User count=%d\n", g_userCount);
	}
};

template <>
struct bluemei::Converter<User*>
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

	static inline Object* toObject(User* user)
	{
		checkNullPtr(user);
		ObjectMap* objMap = new ObjectMap();
		objMap->putValue("name", user->name);
		objMap->putValue("age", user->age);
		//delete user;
		return objMap;
	}
};


template <>
struct bluemei::Converter<User>
{
	static inline User valueOf(Object* obj)
	{
		checkNullPtr(obj);
		ObjectMap* objMap = castAndCheck<ObjectMap*>(obj, "map");

		User user;
		//user->name = objMap["name"]->toString().c_str();
		objMap->getValue("name", user.name);
		objMap->getValue("age", user.age);
		return user;
	}

	static inline Object* toObject(const User& user)
	{
		ObjectMap* objMap = new ObjectMap();
		objMap->putValue("name", user.name);
		objMap->putValue("age", user.age);
		return objMap;
	}
};

User* testReturnUserPtr(cstring name, int age)
{
	User *user = new User();//如何释放?????????
	user->name = name;
	user->age = age;
	return user;
}

SmartPtr<User> testReturnUserSmartPtr(cstring name, int age)
{
	SmartPtr<User> user = new User();
	user->name = name;
	user->age = age;
	return user;
}

User testReturnUser(cstring name, int age)
{
	User user;
	user.name = name;
	user.age = age;
	return user;
}

std::string testGetUserPtrName(User *user)
{
	return user->name;
}

std::string testGetUserSmartPtrName(SmartPtr<User> user)
{
	//SmartPtr<const User> cup;
	return user->name;
}

std::string testGetUserName(User user)
{
	return user.name;
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
struct brpc::Converter<TestClass*>
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
	dispatcher.regFunc(testReturnUserPtr);
	dispatcher.regFunc(testReturnUserSmartPtr);

	dispatcher.regFunc(testGetUserName);
	dispatcher.regFunc(testGetUserPtrName);
	dispatcher.regFunc(testGetUserSmartPtrName);

	try{
		ObjectList args;
		args.addValue("hj");
		args.addValue(27);
		Object* result = dispatcher.call("testReturnUser", args);
		delete result;

		result = dispatcher.call("testReturnUserSmartPtr", args);
		delete result;

		//this will lead to memory leaks
		//result = dispatcher.call("testReturnUserPtr", args);
		//delete result;	
	}catch (Exception& e){
		e.printStackTrace();
	}

	try{
		ObjectList args;
		ObjectMap *arg1 = new ObjectMap();
		arg1->putValue("name","blue");
		arg1->putValue("age",25);
		args.add(arg1);
		Object* result = dispatcher.call("testGetUserPtrName", args);
		delete result;

		result = dispatcher.call("testGetUserName", args);
		delete result;

		result = dispatcher.call("testGetUserSmartPtrName", args);
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
	ObjectList* args = new ObjectList();

	ObjectMap *arg1 = new ObjectMap();
	arg1->putValue("name","blue");
	arg1->putValue("age",25);

	ObjectMap *arg2 = new ObjectMap();
	arg2->putValue("name","green");
	arg2->putValue("age",18);

	args->add(arg1);
	args->add(arg2);
	args->addValue(3.14);
	
	ObjectMap struc;
	struc.put("list1",args);
	//struc.put("list2",&args);

	Type2JsonSerializer v;
	String json = v.toJson(&struc);
	printf("toJson: %s\n", json.c_str());
}


#include "Colume.h"
#include "orm/Session.h"
#include "orm/Functions.h"

class UserModel : public Model
{
public:
	DECLARE_DCLASS(UserModel);
public:
	TABLE(users);
	ID(varchar<32>, "_id");
	COLUME_N(name, varchar<32>, "user_name");
	COLUME(age, int);

	FUNCTION_V(count, lint);
	FUNCTION_C(avg, age);

public:
	String toString() const {
		#define str(c) c.value().value()
		return String::format("User(id=%s, name=%s, age=%d, count=%d)",
			str(_id), str(name), (int)age, (int)count);
	}
};

void testDbUser()
{
	UserModel user;
	user._id = Util::uuid4().c_str();
	user.name = "jack";
	user.age = 18;
	int age = user.age;

	varchar<2> ss("12");
	ss = "13579";
	cstring cstr = ss;

	// Condition
	ConditionWrapper cond = (user.name.query() == "mike") || (user.name.query() == "mike3");
	cond = (user.name.query() == "mike") 
		&& (
		  ((user.age.query() > 1) && (user.age.query() <= 100))
		   ||
		  (user.age.query() >= 200 && user.age.query() != 300.8)
		);
	String sql = cond->toSQL();
	String expect = "((`user_name` = 'mike')"\
		" and (((`age` > 1) and (`age` <= 100)) or ((`age` >= 200) and (`age` != 300.800000))))";
	assert(expect == sql);
	printf("toSQL (Condition): %s\n", sql.c_str());

	// null
	cond = user.name.query() == nullptr && !(user.name.query() != nullptr);
	sql = cond->toSQL();
	printf("toSQL (test null): %s\n\n",  sql.c_str());

	// session
	//TODO: query = user.query(user.name).filter(user.sex == "male");
	brpc::DatabaseConnection db("mysql://127.0.0.1:3306/test"
		"?username=root&password=0315&charset=utf8");
	brpc::Session session(&db);
	
	cstring fields[] = {user.name.fieldName(), user.age.fieldName()};
	size_t len = sizeof(fields) / sizeof(cstring);
	sql = session.query<UserModel>(fields, len).toSQL();
	sql = session.query<UserModel>().toSQL();
	sql = session.query<UserModel>()
		.filter(user.name.query() == "mike")
		.toSQL();
	printf("session.query.toSQL: %s\n\n", sql.c_str());

	// test session
	session.begin();
	session.add(&user);
	session.commit(); // default, the mysql will auto commit!

	session.begin();
	user.age = 17;
	session.update(&user);
	session.rollback();

	user.age = 19;
	session.update(&user);

	auto rs = session.query<UserModel>()
		.filterById(user)
		.all();
	printf("query.Result(filterById): %s\n\n", rs->toString().c_str());

	rs = session.query<UserModel>()
		.query(user.name)
		.query(user.age)
		.filter(user.age)
		.orderBy(user.id())
		.limit(10)
		.offset(3)
		.all();
	printf("query.Result(limit): %s\n\n", rs->toString().c_str());

	rs = session.query<UserModel>()
		.query(user.age)
		.query(user.count)
		.filter(user.age.query() > 18)
		.groupBy(user.age)
		.having(user.count.group() > 1)
		.all();
	printf("query.Result(group): %s\n\n", rs->toString().c_str());

	session.remove(&user);
	
	try {
		session.update(&user);
	} catch (Exception& e) {
		e.printException();
	}
}


class TestTable : public Model
{
public:
	DECLARE_DCLASS(TestTable);
public:
	TABLE(test_table);
	ID(varchar<32>, "_id");
	COLUME8(uuid, varchar<32>, "uuid", true, true, true, "0", true);
	COLUME_N(name, varchar<32>, "user_name");
	COLUME_S(password);
	COLUME(level, int);
	COLUME(sex, bool);

	FUNCTION_V(count, lint);
};

void testCreateDrop()
{
	brpc::DatabaseConnection db("mysql://127.0.0.1:3306/test"
		"?username=root&password=0315&charset=utf8");
	brpc::Session session(&db);

	//database
	bool success = true;
	success = session.createDatabase("db_2");
	assert(success);

	success = session.dropDatabase("db_2");
	assert(success);

	//table
	success = session.createTable(
		"table_test_2(id int not null auto_increment primary key,"
		"name varchar(100) not null)");
	assert(success);

	success = session.dropTable("table_test_2");
	assert(success);

	//table(auto)
	success = session.createTable<TestTable>();
	assert(success);
	success = session.dropTable<TestTable>();
	assert(success);
}

int main(int argc, char* argv[])
{
	_CrtSetBreakAlloc(249);
	cstring s0=CODE2STRING(CLS_PF_OF_ARGS(0));
	cstring s1=CODE2STRING(COLUME(name, varchar<32>));
	cstring s2=CODE2STRING(FUNCTION_C(avg, age));
	
	BRpcUtil::setBrpcDebug(true);

	try {
		testDispatcher();
		testJson();
		testCreateDrop();
		testDbUser();
	} catch (Exception& e) {
		e.printException();
	}

	System::instance().destroy();

	system("pause");
	return 0;
}
