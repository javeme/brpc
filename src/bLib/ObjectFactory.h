#ifndef ObjectFactory_H_H
#define ObjectFactory_H_H
#include "bluemeiLib.h"
#include "Class.h"
#include "Object.h"
#include "RuntimeException.h"

namespace bluemei{

typedef Map<String, Class*> ClassMap;

//Object Factory (there saved class info)
class BLUEMEILIB_API ObjectFactory : public Object
{
public:
	~ObjectFactory(void);
private:
	ObjectFactory(void);
	ObjectFactory(const ObjectFactory&);
	ObjectFactory& operator=(const ObjectFactory&);
public:
	static ObjectFactory& instance();
	static void destroy();
public:
	Object* createObject(cstring className) throw(ClassNotFoundException);
	void registerClass(Class* pClass);
	Class* exist(cstring className);
	void clear();
private:
	ClassMap m_classMap;
private:
	static ObjectFactory* s_objectFactory;
};


//注册动态创建类 用于根据名字创建对象
#if 0
#define REGISTER_CLASS(className)\
{\
	static bool _isRegister=false;\
	if(!_isRegister){\
		/*为何嵌套失效?registerClass(CLASS(className))*/ \
		Class *pClass=className::thisClass();\
		ObjectFactory::instance().registerClass(pClass);\
		_isRegister=true;\
	}\
}
#else
//静态初始化类
typedef struct StaticRegisterClass
{
	StaticRegisterClass(Class *pClass)
	{
		ObjectFactory::instance().registerClass(pClass);
	}
}static_reg_class_t;

#define REGISTER_CLASS(className) StaticRegisterClass register##className(className::thisClass())
#endif

//创建实例
#define newInstanceOf(name) ObjectFactory::instance().createObject(name)

}//end of namespace bluemei

#endif