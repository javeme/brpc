#pragma once

#include "bluemeiLib.h"
#include "RuntimeException.h"

namespace blib{

//模板类无法被dll导出 BLUEMEILIB_API
template <class T>
class BLUEMEILIB_TEMPLATE Vector : public Object// : public vector<T>
{
public:
	Vector(void);
	virtual ~Vector(void);
private:
	std::vector<T> m_dataArray;
public:
	unsigned int add(const T& ele);
	bool insert(unsigned int pos, const T& ele);
	bool remove(unsigned int pos,T& value);
	bool pop(T& value);
	bool remove(const T& ele);
	bool get(unsigned int pos,T& value) const;
	T& operator[](unsigned int pos);
	const T& operator[](unsigned int pos) const;
	unsigned int size() const;
	void clear();
};

}//end of namespace blib