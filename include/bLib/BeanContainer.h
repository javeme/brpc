#pragma once
#include "bluemeiLib.h"
#include "Object.h"

namespace bluemei{

using std::string;

class BLUEMEILIB_API BeanContainer : public Object
{
public:
	typedef std::map<string,Object*> ObjectMap;

	BeanContainer(void);
	virtual ~BeanContainer(void);
protected:
	BeanContainer(const BeanContainer& other){
		*this=other;
	};
	BeanContainer& operator=(const BeanContainer& other){
		__super::operator=(other);
		m_beanMap=other.m_beanMap;
		return *this;
	};
public:
	virtual bool put(const string& key,Object* pBean);
	/*bool put(const char* key,Object* pBean){
		return put(string(key),pBean);
	}*/
	virtual Object* get(const string& key) const;
	virtual Object* remove(const string& key);
	virtual bool remove(Object* pBean);

	virtual int size() const{ return m_beanMap.size(); };

	virtual void clear(bool autoRelease=true);

	virtual Object* operator[](const string& key) const;
	virtual Object*& operator[](const string& key);
protected:
	ObjectMap m_beanMap;
};

}//end of namespace bluemei