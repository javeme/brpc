#pragma once
#include "stdafx.h"
#include "BRpcUtil.h"
#include "TypeVisitor.h"


namespace brpc{

/*
* Object对象列表
* @author 李章梅
* @create 2014/7/4
*/
class ObjectList : public Vector<Object*>, public TypeVisitable
{
public:
	ObjectList();
	virtual ~ObjectList();
public:
	ObjectList(const ObjectList& other){ *this = other; }
	ObjectList& operator=(const ObjectList& other);
public:
	template<typename Type>
	void addValue(const Type& val){ 
		this->add(brpc::toObject(val));
	}

	template<typename Type>
	bool getValue(unsigned int index, Type& val)const{ 
		Object* obj = null;
		if (this->get(index, obj))
		{
			val = brpc::valueOf<Type>(obj);
			return true;
		}
		return false;
	}

	unsigned int addValue(cstring val){ 
		return this->add(toObject(val));
	}

	virtual bool remove(unsigned int index, bool del=true);
	virtual void destroy(bool del=true);

	Object* operator[](unsigned int pos) const;
	virtual String toString() const;
public:
	class Iterator : public TypeIterator
	{
	public:
		Iterator(Object* obj):value(obj){}
		Object* value;
	};
	virtual void accept(TypeVisiter* visitor);
	virtual void visitElements(TypeVisiter* visitor);
protected:
	bool autoDelete;
};


/*函数模板无法偏特化!!!
template <typename T>
inline std::vector<T> valueOf<std::vector<T>>(Object* obj)
{
	checkNullPtr(obj);
	ObjectList* pList = dynamic_cast<ObjectList*>(obj);
	if (pList == null)
	{
		throwpe(TypeNotMatchedException(obj->toString() + " not matches <type vector>"))
	}

	std::vector<T> vec;
	ObjectList& list = *pList;
	for (unsigned int i=0; i<list.size(); i++)
	{
		vec.push_back(valueOf<T>(list[i]));
	}
	return vec;
}*/

template <typename T>
struct Converter<std::vector<T>>
{
	static inline std::vector<T> valueOf(Object* obj)
	{
		checkNullPtr(obj);
		ObjectList* pList = castAndCheck<ObjectList*>(obj, "vector");

		std::vector<T> vec;
		ObjectList& list = *pList;
		for (unsigned int i=0; i<list.size(); i++)
		{
			vec.push_back(brpc::valueOf<T>(list[i]));
		}
		return vec;
	}

	static inline Object* toObject(const std::vector<T>& val)
	{
		ObjectList* list = new ObjectList();
		for (unsigned int i=0; i<val.size(); i++)
		{
			list->addValue(val[i]);
		}
		return list;
	}
};


}//end of namespace bluemei