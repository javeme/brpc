#pragma once
#include "Vector.h"

namespace bluemei{

template <class T>
Vector<T>::Vector(void)
{
	//dataArray.resize(10);
}

template <class T>
Vector<T>::~Vector(void)
{
	;
}

template <class T>
unsigned int Vector<T>::add(const T& ele)
{
	m_dataArray.push_back(ele);
	return this->size();
}

template <class T>
bool Vector<T>::insert(unsigned int pos, const T& ele)
{
	if(pos<0||pos>m_dataArray.size())
		return false;
	m_dataArray.insert(m_dataArray.begin()+pos, ele);
	return true;
}

template <class T>
bool Vector<T>::remove(unsigned int pos,T& value)
{
	if(pos<0||pos>=m_dataArray.size())
		return false;
	std::vector <T> ::iterator iter = m_dataArray.begin();
	advance(iter,pos);
	value=*iter;
	m_dataArray.erase(iter);
	return true;
}

template <class T>
bool Vector<T>::pop(T& value)
{
	return remove(size()-1, value);
}

template <class T>
bool Vector<T>::get(unsigned int pos,T& value)const
{
	if(pos<0||pos>=m_dataArray.size())
		return false;
	value=m_dataArray[pos];
	return true;
}

template <class T>
T& Vector<T>::operator[](unsigned int pos)
{
	if(pos<0||pos>=m_dataArray.size())
	{
		char buf[100];
		sprintf_s(buf,"Vector size is %d, can't access %d.",m_dataArray.size(),pos);
		throw OutOfBoundException(buf);
	}
	return m_dataArray[pos];
}

template <class T>
const T& Vector<T>::operator[](unsigned int pos)const
{
	if(pos<0||pos>=m_dataArray.size())
	{
		char buf[100];
		sprintf_s(buf,"Vector size is %d, can't access %d.",m_dataArray.size(),pos);
		throw OutOfBoundException(buf);
	}
	return m_dataArray[pos];
}

template <class T>
bool Vector<T>::remove(const T& ele)
{
	std::vector<T>::iterator iter = find(m_dataArray.begin(), m_dataArray.end(), ele);
	if (iter != m_dataArray.end())
	{
		m_dataArray.erase(iter);
		return true;
	}
	return false;
}

template <class T>
unsigned int Vector<T>::size() const
{
	return m_dataArray.size();
}

template <class T>
void Vector<T>::clear()
{
	m_dataArray.clear();
}

}//end of namespace bluemei