#pragma once
#include "stdafx.h"
#include "BRpcUtil.h"
#include "TypeVisitor.h"


namespace brpc{

/*
* Object∂‘œÛMap
* @author Javeme
* @create 2014/7/15
*/
class ObjectMap : public BeanContainer, public TypeVisitable
{
public:
	ObjectMap(cstring classType="") : classType(classType){}
	virtual ~ObjectMap(){ destroy(); }
private:
	ObjectMap(const ObjectMap& other){ *this = other; }
	ObjectMap& operator=(const ObjectMap& other){ return *this; }
public:
	template<typename Type>
	bool putValue(cstring name, const Type& val){ 
		Object* obj = brpc::toObject(val);
		bool sucess = this->put(name, obj);
		if(!sucess)
			delete obj;
		return sucess;
	}

	template<typename Type>
	bool getValue(cstring name, Type& val)const{ 
		Object* obj = this->get(name);
		if (obj)
		{
			val = brpc::valueOf<Type>(obj);
			return true;
		}
		return false;
	}

	bool putValue(cstring name, cstring val){ 
		Object* obj = brpc::toObject(val);
		bool sucess = this->put(name, obj);
		if(!sucess)
			delete obj;
		return sucess;
	}

	bool putValue(cstring name, const String& val){ 
		return this->putValue(name, val.c_str());
	}

	virtual Object* remove(cstring name, bool del=true);

	virtual void destroy(){ clear(); };

	Object* operator[](cstring name) const
	{
		return this->get(name);
	}
	Object*& operator[](cstring name)
	{
		return BeanContainer::operator [](name);
	}
	
	virtual String toString() const;
public:
	typedef BeanContainer::ObjectMap::const_iterator MapIterator;
	MapIterator begin() const{ return m_beanMap.begin(); }
	MapIterator end() const{ return m_beanMap.end(); }

public:
	class Iterator : public TypeIterator
	{
	public:
		Iterator(cstring key, Object* obj):key(key),value(obj){}
		cstring key;
		Object* value;
	};

	virtual void accept(TypeVisiter* visitor) 
	{
		checkNullPtr(visitor);
		visitor->visit(this);
	}
	virtual void visitElements(TypeVisiter* visitor)
	{
		checkNullPtr(visitor);
		for (auto itor = begin(); itor != end(); ++itor)
		{
			const std::string& name = itor->first;
			Object* obj = itor->second;
			Iterator typeIterator(name.c_str(), obj);
			typeIterator.accept(visitor);
		}
	}

	String getClassType() const { return classType; }
	void setClassType(cstring val) { classType = val; }
protected:
	String classType;
};


template <typename T>
struct Converter<std::map<std::string,T>>
{
	static inline std::map<std::string,T> valueOf(Object* obj)
	{
		checkNullPtr(obj);
		ObjectMap* pMap = castAndCheck<ObjectMap*>(obj, "map");

		std::map<std::string,T> map;
		ObjectMap& objMap = *pMap;
		for (auto itor = objMap.begin(); itor != objMap.end(); ++itor)
		{	
			map.insert(make_pair(itor->first, brpc::valueOf<T>(itor->second)));
		}
		return map;
	}

	static inline Object* toObject(const std::map<std::string,T>& val)
	{
		ObjectMap* objMap = new ObjectMap();
		for (auto itor = val.begin(); itor != val.end(); ++itor)
		{	
			objMap->putValue(itor->first.c_str(), itor->second);
		}
		return objMap;
	}
};

//map << key1: value1 << key2: value2;

}//end of namespace brpc