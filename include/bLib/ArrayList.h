#ifndef ArrayList_H_H
#define ArrayList_H_H
#include "bluemeiLib.h"
#include "Object.h"
#include "RuntimeException.h"

namespace blib{

template< class T >
class BLUEMEILIB_TEMPLATE ArrayList : public Object
{
public:
	ArrayList(unsigned int size=0,float capacityFactor=1.2f);

	ArrayList(const ArrayList<T>& other);
	ArrayList<T>& operator=(const ArrayList<T>& other);

	ArrayList(ArrayList<T>&& other);//move
	ArrayList<T>& operator=(ArrayList<T>&& other);

	virtual ~ArrayList();
public:
	virtual T& get(unsigned int i) const;
	virtual T& getLast() const;
	virtual T& getFirst() const;

	virtual unsigned int find(const T& v,unsigned int i=0);//查找元素位置

	virtual void set(unsigned int i,const T& v);//设置指定位置的元素值
	virtual unsigned int add(const T& v);//添加元素,返回添加到的index

	virtual unsigned int remove(const T& v);//删除元素,返回删除的index
	virtual unsigned int removeAt(unsigned int i,T& v);//删除指定位置元素,返回剩下元素数量
	virtual unsigned int removeLast(unsigned int n);//删除n个元素,返回剩下元素数量

	virtual void clear();

	virtual unsigned int size() const;
	virtual void resize(unsigned int size,float capacityFactor=1.2f);
	virtual void resetCapacity(unsigned int capacity);

	virtual void copy(const ArrayList<T>& list, unsigned int to, unsigned int num);

	const T& operator[](int pos) const { return get(pos); }
	T& operator[](int pos) { return get(pos); }

	virtual String toString() const;
protected:
	virtual void assign(unsigned int capacity);
	virtual void release();
	virtual void checkBound(unsigned int current) const;
protected:
	T *m_pData;
	unsigned int m_nSize;
	unsigned int m_nCapacity;
};


template< class T >
ArrayList<T>::ArrayList(unsigned int size,float capacityFactor)
{
	unsigned int capacity=8;
	if(size>capacity)
	{
		if(capacityFactor<1.0)
			capacityFactor=1.0;
		capacity=(unsigned int)(size*capacityFactor);
	}
	assign(capacity);
	m_nSize=size;
}

template< class T >
ArrayList<T>::ArrayList( const ArrayList<T>& other )
{
	assign((unsigned int)(other.size()*1.2));
	m_nSize=0;
	this->operator=(other);
}

template< class T >
ArrayList<T>& ArrayList<T>::operator=( const ArrayList<T>& other )
{
	if(this!=&other)
		copy(other,0,other.size());//如果是指针则覆盖后可能导致内存泄露
	return *this;
}

template< class T >
ArrayList<T>::ArrayList(ArrayList<T>&& other)
{
	m_pData=nullptr;
	m_nSize=m_nCapacity=0;
	this->operator=(move(other));
}

template< class T >
ArrayList<T>& ArrayList<T>::operator=( ArrayList<T>&& other )
{
	if(this!=&other)
	{
		release();
		m_pData=other.m_pData;
		m_nSize=other.m_nSize;
		m_nCapacity=other.m_nCapacity;

		other.m_pData=nullptr;
		other.m_nSize=0;
		other.m_nCapacity=0;
		other.assign(2);
	}
	return *this;
}

template< class T >
ArrayList<T>::~ArrayList()
{
	release();
	m_nSize=0;
}


template< class T >
T& ArrayList<T>::get(unsigned int i) const
{
	checkBound(i);
	return m_pData[i];
}

template< class T >
T& ArrayList<T>::getFirst() const
{
	return get(0);
}

template< class T >
T& ArrayList<T>::getLast() const
{
	return get(m_nSize-1);
}

template< class T >
unsigned int ArrayList<T>::find( const T& v,unsigned int i/*=0*/ )
{
	checkBound(i);

	for(unsigned int j=i;j<m_nSize;j++)
	{
		if(m_pData[j]==v)
		{
			return j;
		}
	}
	return -1;
}

template< class T >
void ArrayList<T>::set(unsigned int i,const T& v)
{
	checkBound(i);
	//if(m_nCurrent<=i)
	//	m_nCurrent=i+1;
	m_pData[i]=v;
}

template< class T >
unsigned int ArrayList<T>::add( const T& v )
{
	//checkBound(m_nCurrent);
	if(m_nSize>=m_nCapacity)
	{
		resetCapacity((unsigned int)(1.5*m_nSize));
	}
	m_pData[m_nSize++]=v;
	return m_nSize-1;
}

template< class T >
unsigned int ArrayList<T>::removeAt( unsigned int i, T& v )
{
	checkBound(i);

	v=m_pData[i];
	--m_nSize;
	for(unsigned int j=i;j<m_nSize;j++)
	{
		m_pData[j]=m_pData[j+1];
	}
	resetMemory<T>(m_pData+m_nSize,1);
	return m_nSize;
}

template< class T >
unsigned int ArrayList<T>::remove( const T& v )
{
	unsigned int index=find(v);
	if(index!=-1)
	{
		T v;
		removeAt(index,v);
	}

	return index;
}

template< class T >
unsigned int ArrayList<T>::removeLast( unsigned int n )
{
	checkBound(n);
	m_nSize=m_nSize-n;
	resetMemory<T>(m_pData+m_nSize,n);
	return m_nSize;
}

template< class T >
void ArrayList<T>::clear()
{
	resetMemory<T>(m_pData,m_nSize);
	m_nSize=0;
}

//////////////////////////////////////////////////////////////////////////////////////
//数据复制
template< class T >
class BLUEMEILIB_TEMPLATE copyData
{
public:
	copyData(T* to, const T* from, unsigned int count)
	{
		for(unsigned int i=0;i<count;i++)//适用于基本数据类型及class类型的复制
		{
			to[i]=from[i];
		}
	}
};

//内存清零
template< class T >
class BLUEMEILIB_TEMPLATE resetMemory
{
public:
	resetMemory(T* buf, unsigned int count)
	{
		String type=typeid(T).name();
		bool isPoiner=type.contain("*");//T是否为指针类型

		for(unsigned int i=0;i<count;i++)//适用于基本数据类型及class类型的"清零"
		{
			if(isPoiner)//指针
			{
				buf[i]=T();//(T)nullptr
			}
			else//对象
			{
				buf[i]=T();
			}
		}
	}
};

//基本数据类型复制的优化
#define LIST_COPY_BASE_TYPE(t)\
template<> copyData<t>::copyData(t* to, const t* from, unsigned int count){\
	memcpy(to,from,count*sizeof(t));/*只能用于基本数据类型的复制*/\
}\
template class BLUEMEILIB_API copyData<t>;\
template<> resetMemory<t>::resetMemory(t* buf, unsigned int count){\
	memset(buf,0,count*sizeof(t));/*只能用于基本数据类型的清零*/\
}\
template class BLUEMEILIB_API resetMemory<t>;
//////////////////////////////////////////////////////////////////////////////////////

template< class T >
void ArrayList<T>::copy( const ArrayList<T>& list, unsigned int to, unsigned int num)
{
	//checkBound(to+num);
	if(to+num>m_nCapacity)
	{
		this->resetCapacity((unsigned int)((to+num)*1.2));
	}
	copyData<T>(m_pData+to,list.m_pData,num);
	/*
	for(int i=0;i<num;i++)//适用于基本数据类型及class类型的复制
	{
		m_pData[to+i]=list.m_pData[i];
	}
	//memcpy(m_pData+to,list.m_pData,num*sizeof(T));//适用于基本数据类型
	*/
	m_nSize=to+num;
}

/*
//优化基本数据类型的拷贝(貌似要放在cpp中去才行,暂时不用)
#define LIST_COPY_BASE_TYPE(t)\
	template<> void ArrayList<t>::copy(const ArrayList<t>& list, unsigned int to, unsigned int num){\
	if(to+num>m_nSize)\
	this->resize((to+num)*1.2);\
	memcpy(m_pData+to,list.m_pData,num*sizeof(t));\
	m_nCurrent=to+num;\
}
*/

template< class T >
void ArrayList<T>::assign( unsigned int capacity )
{
	if(capacity<2)
		capacity=2;
	m_nCapacity=capacity;
	m_pData = new T[capacity];
	resetMemory<T>(m_pData,capacity);
}

template< class T >
void ArrayList<T>::release()
{
	delete[] m_pData;
	m_pData=nullptr;
	m_nCapacity=0;
}

template< class T >
unsigned int ArrayList<T>::size() const
{
	return m_nSize;
}

template< class T >
void ArrayList<T>::resize(unsigned int size ,float capacityFactor)
{
	if (size>=m_nCapacity)
	{
		if(capacityFactor<1.0)
			capacityFactor=1.0;
		resetCapacity((unsigned int)(size*capacityFactor));
	}
	m_nSize=size;
}

template< class T >
void ArrayList<T>::resetCapacity( unsigned int capacity )
{
	if(capacity<m_nCapacity)
		return;
	m_nCapacity=capacity;
	T* newBuf=new T[m_nCapacity];
	//memcpy(newBuf,m_pData,m_nSize*sizeof(T));
	copyData<T>(newBuf,m_pData,m_nSize);
	delete[] m_pData;
	m_pData=newBuf;
}

template< class T >
void ArrayList<T>::checkBound(unsigned int current) const
{
	if(current>=m_nSize || current>=m_nCapacity)
	{
		throw OutOfBoundException(current,m_nSize);
	}
}

template< class T >
String ArrayList<T>::toString() const
{
	String str=String::format("ArrayList size=%d{",size());
	for(unsigned int i=0;i<size();i++)
	{
		String s=Value2String<T>(get(i));
		str+=String::format("[%d: %s],",i,s.c_str());
	}
	if(size()>0)
		((char*)str.c_str())[str.length()-1]='}';
	else
		str+="}";
	return str;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXPORT_LIST_OF(t) \
	template class BLUEMEILIB_API ArrayList<t>
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXPORT_LIST_OF_BASE_TYPE(t) \
	LIST_COPY_BASE_TYPE(t);\
	template class BLUEMEILIB_API ArrayList<t>;
#define EXPORT_LIST_OF2(t) \
	EXPORT_LIST_OF_BASE_TYPE(t); EXPORT_LIST_OF_BASE_TYPE(unsigned t)
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EXPORT_LIST_OF2(char);
EXPORT_LIST_OF2(short);
EXPORT_LIST_OF2(int);
EXPORT_LIST_OF2(long);
EXPORT_LIST_OF2(long long);
EXPORT_LIST_OF_BASE_TYPE(bool);
EXPORT_LIST_OF_BASE_TYPE(float);
EXPORT_LIST_OF_BASE_TYPE(double);


}//end of namespace blib
#endif
