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
DataBuffer<Type>::DataBuffer(unsigned int capacity,unsigned int size)
{
	if(capacity<1)
		capacity=1;
	if(capacity<size)
		capacity=size;
	this->m_nCapacity=capacity+(8-capacity%8);
	this->m_buffer=new Type[m_nCapacity];

	memClear(m_buffer,capacity);

	this->m_nSize=size;
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
		this->m_nCapacity=other.m_nCapacity;
		this->m_nSize=other.m_nSize;
		this->m_nReadOffset=other.m_nReadOffset;
		if(m_buffer!=nullptr)
			delete[] m_buffer;
		this->m_buffer=new Type[m_nCapacity];
		memClear(m_buffer,m_nCapacity);
		copyData<Type>(m_buffer,other.m_buffer,other.m_nSize);
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
		this->m_nCapacity=other.m_nCapacity;
		this->m_nSize=other.m_nSize;
		this->m_nReadOffset=other.m_nReadOffset;
		if(m_buffer!=nullptr)
			delete[] m_buffer;
		this->m_buffer = other.m_buffer;
		
		other.m_nCapacity = 2;
		other.m_buffer = new Type[other.m_nCapacity];
		memClear(other.m_buffer,other.m_nCapacity);
		other.m_nSize = 0;
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
bool DataBuffer<Type>::increaseCapacity(unsigned int newCapacity)
{
	if(newCapacity<=m_nCapacity)
		return false;
	Type* newBuffer=new Type[newCapacity];
	copyData<Type>(newBuffer,m_buffer,m_nCapacity);
	delete[] m_buffer;
	m_buffer=newBuffer;
	m_nCapacity=newCapacity;
	return true;
}

template<typename Type>
void DataBuffer<Type>::ensureCapacity(unsigned int append)
{
	unsigned int len=append;
	//need increase capacity
	if(m_nSize+len>=m_nCapacity)
	{
		len=len+(8-len%8);
		if(!increaseCapacity(m_nCapacity*2+len))
		{
			throw OutOfBoundException("DataBuffer::ensureCapacity(): can't increase capacity.");
		}
	}
}

template<typename Type>
unsigned int DataBuffer<Type>::put(const Type& value)
{
	ensureCapacity(1);
	m_buffer[m_nSize++]=value;
	return m_nSize-1;
}

template<typename Type>
unsigned int DataBuffer<Type>::put(const Type values[],unsigned int len)
{
	unsigned int oldOffset=m_nSize;
	ensureCapacity(len);
	copyData<Type>(m_buffer+m_nSize,values,len);
	m_nSize+=len;
	return oldOffset;
}

//setReadOffset() => put2ReadPos() => read()
template<typename Type>
unsigned int DataBuffer<Type>::put2ReadPos(const Type values[],unsigned int len)
{
	unsigned int oldOffset=m_nReadOffset;
	ensureCapacity(len);
	copyData<Type>(m_buffer+m_nReadOffset,values,len);
	m_nReadOffset+=len;
	return oldOffset;
}

template<typename Type>
const Type& DataBuffer<Type>::pop() throw(OutOfBoundException)
{
	if(m_nSize>0){
		m_nSize--;
		return m_buffer[m_nSize];
	}
	else{
		throw OutOfBoundException("DataBuffer::pop(): there is no element any more.");
	}
}

template<typename Type>
void DataBuffer<Type>::read(Type values[],unsigned int len) throw(OutOfBoundException)
{
	if(m_nReadOffset+len>m_nSize)
		throw OutOfBoundException(m_nReadOffset+len,m_nSize);
	copyData<Type>(values,m_buffer+m_nReadOffset,len);
	m_nReadOffset+=len;
}

template<typename Type>
Type DataBuffer<Type>::read() throw(OutOfBoundException)
{
	if(m_nReadOffset>=m_nSize)
		throw OutOfBoundException(m_nReadOffset,m_nSize);
	return m_buffer[m_nReadOffset++];
}

template<typename Type>
void DataBuffer<Type>::setReadOffset(unsigned int offset)
{
	if(offset>=m_nSize)
		throw OutOfBoundException(m_nReadOffset,m_nSize);
	m_nReadOffset=offset;
}

template<typename Type>
void bluemei::DataBuffer<Type>::skipn(unsigned int bytes)
{
	setReadOffset(m_nReadOffset+bytes);
}

//remove the first @bytes data
template<typename Type>
void bluemei::DataBuffer<Type>::shrink(unsigned int bytes)
{
	if(bytes==0)
		return;
	if(bytes>m_nReadOffset)
		bytes=m_nReadOffset;

	unsigned int remain = m_nSize - bytes;
	memmove(m_buffer, ((char*)m_buffer)+bytes, remain);
	this->m_nReadOffset -= bytes;
	this->m_nSize -= bytes;
}

template<typename Type>
bool DataBuffer<Type>::set(const Type& value,unsigned int index)
{
	if(index<m_nCapacity)
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
	if(index<m_nSize)
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
	this->m_nSize=0;
	this->m_nReadOffset=0;
	memClear(m_buffer,m_nCapacity);
}


template<typename Type>
unsigned int DataBuffer<Type>::remains()const
{
	if(m_nSize<=m_nReadOffset)
		return 0;
	return m_nSize-m_nReadOffset;
}

template<typename Type>
unsigned int DataBuffer<Type>::size() const
{
	return this->m_nSize;
}

template<typename Type>
const Type* DataBuffer<Type>::array() const
{
	return this->m_buffer;
}

//导出字节缓冲类
template class BLUEMEILIB_API DataBuffer<byte>;

}//end of namespace bluemei