#pragma once
#include "stdafx.h"
#include "DataBuffer.h"
#include "Exception.h"
#include "ArrayList.h"

namespace bluemei{

/*
template<typename Type,int size=0>
struct memClear{
	void operator()(Type* buffer,unsigned int count){
		memset(buffer,0,size*count);
	}
};
#define REG_TYPE(T) \
	template<>struct memClear<T,sizeof(T)>;\
	template<>struct memClear<unsigned T,sizeof(unsigned T)>;
template<>struct memClear<Object,0>;
REG_TYPE(char);
REG_TYPE(short);
REG_TYPE(int);
REG_TYPE(long);
REG_TYPE(long long);
REG_TYPE(float);
REG_TYPE(double);
*/
template<typename Type>
struct BLUEMEILIB_TEMPLATE clearTypeSize{
	enum{typeSize=0};
	size_t operator()(void){
		return typeSize;
	}
};
#define REG_TYPE(T) \
	template<> struct BLUEMEILIB_API clearTypeSize<T>{ enum{typeSize=sizeof(T)}; };
#define REG_2TYPE(T) REG_TYPE(T) REG_TYPE(unsigned T)

REG_2TYPE(char);
REG_2TYPE(short);
REG_2TYPE(int);
REG_2TYPE(long);
REG_2TYPE(long long);
REG_TYPE(float);
REG_TYPE(double);

template<typename Type>
void memClear(Type* buffer,unsigned int count){//BLUEMEILIB_TEMPLATE 
	memset(buffer,0,clearTypeSize<Type>::typeSize*count);
}

template<typename Type>
DataBuffer<Type>::DataBuffer(unsigned int maxSize,unsigned int size)
{
	if(maxSize<2)
		maxSize=2;
	if(maxSize<size)
		maxSize=size;
	this->m_nMaxSize=maxSize;
	this->m_buffer=new Type[m_nMaxSize];

	memClear(m_buffer,maxSize);

	this->m_nOffset=size;
	this->m_nReadOffset=0;
}

template<typename Type>
DataBuffer<Type>::DataBuffer(const DataBuffer<Type>& other)
{
	this->m_buffer=nullptr;
	*this=other;
}

template<typename Type>
DataBuffer<Type>& DataBuffer<Type>::operator=(const DataBuffer<Type>& other)
{
	if(this!=&other)
	{
		this->m_nMaxSize=other.m_nMaxSize;
		this->m_nOffset=other.m_nOffset;
		this->m_nReadOffset=other.m_nReadOffset;
		if(m_buffer!=nullptr)
			delete[] m_buffer;
		this->m_buffer=new Type[m_nMaxSize];
		memClear(m_buffer,m_nMaxSize);
		copyData<Type>(m_buffer,other.m_buffer,other.m_nOffset);
	}
	return *this;
}

template<typename Type>
DataBuffer<Type>::DataBuffer(DataBuffer<Type>&& other)
{
	this->m_buffer=nullptr;
	*this = std::move(other);
}

template<typename Type>
DataBuffer<Type>& DataBuffer<Type>::operator=(DataBuffer<Type>&& other)
{
	if(this!=&other)
	{
		this->m_nMaxSize=other.m_nMaxSize;
		this->m_nOffset=other.m_nOffset;
		this->m_nReadOffset=other.m_nReadOffset;
		if(m_buffer!=nullptr)
			delete[] m_buffer;
		this->m_buffer = other.m_buffer;
		
		other.m_nMaxSize = 2;
		other.m_buffer = new Type[other.m_nMaxSize];
		memClear(other.m_buffer,other.m_nMaxSize);
		other.m_nOffset = 0;
		other.m_nReadOffset = 0;
	}
	return *this;
}


template<typename Type>
DataBuffer<Type>::~DataBuffer(void)
{
	delete[] m_buffer;
}

template<typename Type>
bool DataBuffer<Type>::increaseCapacity( unsigned int newCapacity )
{
	if(newCapacity<=m_nMaxSize)
		return false;
	Type* newBuffer=new Type[newCapacity];
	copyData<Type>(newBuffer,m_buffer,m_nMaxSize);
	delete[] m_buffer;
	m_buffer=newBuffer;
	m_nMaxSize=newCapacity;
	return true;
}

template<typename Type>
unsigned int DataBuffer<Type>::put(const Type& value)
{
	m_buffer[m_nOffset++]=value;
	//扩容
	if(m_nOffset>=m_nMaxSize)
	{
		increaseCapacity(m_nMaxSize*2);
	}
	return m_nOffset-1;
}

template<typename Type>
unsigned int DataBuffer<Type>::put( const Type values[],unsigned int len )
{
	unsigned int oldoffset=m_nOffset;
	//扩容
	if(m_nOffset+len>=m_nMaxSize)
	{
		increaseCapacity(m_nMaxSize*2+len);
	}
	copyData<Type>(m_buffer+m_nOffset,values,len);
	m_nOffset+=len;
	return oldoffset;
}

template<typename Type>
unsigned int DataBuffer<Type>::put2ReadPos( const Type values[],unsigned int len )
{
	unsigned int oldoffset=m_nReadOffset;
	//检查是否越界
	if(m_nReadOffset+len>m_nOffset)
	{
		throw OutOfBoundException("DataBuffer::put2ReadPos(): there is no element any more.");
	}
	copyData<Type>(m_buffer+m_nReadOffset,values,len);
	m_nReadOffset+=len;
	return oldoffset;
}

template<typename Type>
const Type& DataBuffer<Type>::pop() throw(OutOfBoundException)
{
	if(m_nOffset>0){
		m_nOffset--;
		return m_buffer[m_nOffset];
	}
	else{
		throw OutOfBoundException("DataBuffer::pop(): there is no element any more.");
	}
}

template<typename Type>
void DataBuffer<Type>::read( Type values[],unsigned int len ) throw(OutOfBoundException)
{
	if(m_nReadOffset+len>m_nOffset)
		throw OutOfBoundException(m_nReadOffset+len,m_nOffset);
	copyData<Type>(values,m_buffer+m_nReadOffset,len);
	m_nReadOffset+=len;
}

template<typename Type>
Type DataBuffer<Type>::read() throw(OutOfBoundException)
{
	if(m_nReadOffset>=m_nOffset)
		throw OutOfBoundException(m_nReadOffset,m_nOffset);
	return m_buffer[m_nReadOffset++];
}

template<typename Type>
void DataBuffer<Type>::setReadOffset(unsigned int offset)
{
	m_nReadOffset=offset;
}

template<typename Type>
bool DataBuffer<Type>::set(const Type& value,unsigned int index)
{
	if(index<m_nMaxSize)
	{
		m_buffer[index]=value;
		return true;
	}
	else
		return false;
}

template<typename Type>
bool DataBuffer<Type>::get(Type& value,unsigned int index)
{
	if(index<m_nOffset)
	{
		value=m_buffer[index];
		return true;
	}
	else
		return false;
}

template<typename Type>
void DataBuffer<Type>::clear()
{
	this->m_nOffset=0;
	memClear(m_buffer,m_nMaxSize);
}


template<typename Type>
unsigned int DataBuffer<Type>::remainBytes()const
{
	if(m_nOffset<=m_nReadOffset)
		return 0;
	return m_nOffset-m_nReadOffset;
}

template<typename Type>
unsigned int DataBuffer<Type>::size() const
{
	return this->m_nOffset;
}

template<typename Type>
const Type* DataBuffer<Type>::array() const
{
	return this->m_buffer;
}

//导出字节缓冲类
template class BLUEMEILIB_API DataBuffer<byte>;

}//end of namespace bluemei