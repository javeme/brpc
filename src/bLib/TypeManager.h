#pragma once
#include "Object.h"
#include "HashMap.h"
#include "CriticalLock.h"

namespace bluemei{

class Conver : public Object
{
public:
	virtual Object* cast(void* p)=0;
};

template<typename Self>
class ConvObject : public Conver
{
public:
	ConvObject():typeInfo(typeid(Self)){}
public:
	/*template<typename T>
	Self* operator()(T* v){
		return static_cast<Self*>(v);
	}*/
	Self* operator()(void* v){
		return static_cast<Self*>(v);
	}

	template<typename Target>
	Target* cast(void* v){
		//return static_cast<Target*>(this->operator()(v));//Target*为char*之类的时候会报错
		return (Target*)(this->operator()(v));
	}
	template<typename Target>
	Target* dcast(void* v){
		return dynamic_cast<Target*>(this->operator()(v));
	}

	virtual Object* cast(void* p){
		return cast<Object>(p);
	}
private:
	const type_info& typeInfo;
};

//1.找到本身的对象类型
//2.static_cast为本身类型
//3.dynamic_cast为目标类型
class BLUEMEILIB_API TypeManager : public Object
{
public:
	typedef const type_info* TypeInfo;

	static TypeInfo getTypeInfo(void* p){
		Object* obj=(Object*)p;
		return &(typeid(*obj));
	}
	template<typename T>
	static TypeInfo getTypeInfo(){
		return &(typeid(T));
	}

public:
	static TypeManager* instance();
	static void releaseInstance();
protected:
	static CriticalLock lock;
	static TypeManager *thisInstance;

private:
	TypeManager(){}
	virtual ~TypeManager(){
		destroy();
	}
public:
	template<typename T>
	bool registerType(){
		auto k=getTypeInfo<T>();
		if(!typeMap.contain(k))
		{
			auto v=new ConvObject<T>();
			return typeMap.put(k,v);
		}
		return false;
	}

	void destroy();

	template<typename Target>
	Target* cast(void* p){//注意:传入参数必须为本来类型的指针,不能为父类指针!(因为p指向的地址必须为本身对象的地址,若不是,在多继承时会出错)
		if (p==nullptr)
			return nullptr;
		auto conver=typeMap.get(getTypeInfo(p));
		if(conver==nullptr)
			return (Target*)p;
		//return static_cast<Target*>((*conver)->cast(p));//Target*为char*之类的时候会报错
		return (Target*)((*conver)->cast(p));
	}
	template<typename Target>
	Target* dcast(void* p){
		if (p==nullptr)
			return nullptr;	
		auto conver=typeMap.get(getTypeInfo(p));
		if(conver==nullptr)
			return (Target*)p;
		return dynamic_cast<Target*>((*conver)->cast(p));
	}


protected:
	HashMap<TypeInfo,Conver*> typeMap;
};


}//end of namespace bluemei