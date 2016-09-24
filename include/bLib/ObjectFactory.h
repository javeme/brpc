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


//类注册器 用于根据名字创建对象
typedef struct StaticRegisterClass
{
	StaticRegisterClass(Class *pClass)
	{
		if(!ObjectFactory::instance().exist(pClass->getName()))
			ObjectFactory::instance().registerClass(pClass);
	}
}static_reg_class_t;

//类注册宏
#define REGISTER_CLASS(className) \
	static StaticRegisterClass register##className(className::thisClass())

//创建实例
#define newInstanceOf(name) ObjectFactory::instance().createObject(name)

}//end of namespace bluemei

#endif