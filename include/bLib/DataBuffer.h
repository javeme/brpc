#pragma once
#include "bluemeiLib.h"
#include "Object.h"
#include "RuntimeException.h"

namespace bluemei{

template<typename Type>
class BLUEMEILIB_TEMPLATE DataBuffer : public Object//BLUEMEILIB_API
{
public:
	DataBuffer(unsigned int capacity=8,unsigned int size=0);
	DataBuffer(const DataBuffer<Type>& other);
	DataBuffer<Type>& operator=(const DataBuffer<Type>& other);
	virtual ~DataBuffer(void);
public:
	DataBuffer(DataBuffer<Type>&& other);
	DataBuffer<Type>& operator=(DataBuffer<Type>&& other);
public:	
	virtual unsigned int put(const Type& value);
	virtual unsigned int put(const Type values[],unsigned int len);
	virtual unsigned int put2ReadPos(const Type values[],unsigned int len);//从读偏移m_nReadOffset处写入若干数据(不能超出m_nOffset)

	virtual const Type& pop() throw(OutOfBoundException);
	
	virtual void read(Type values[],unsigned int len) throw(OutOfBoundException);
	virtual Type read() throw(OutOfBoundException);
	virtual void setReadOffset(unsigned int offset=0);
	virtual void skipn(unsigned int bytes);
	virtual void shrink(unsigned int bytes);

	virtual bool set(const Type& value,unsigned int index);
	virtual bool get(Type& value,unsigned int index);

	virtual bool set(const Type& value){ return set(value,m_nSize-1); }
	virtual bool get(Type& value){ return get(value,m_nReadOffset); }
	
	virtual void clear();
	
	virtual unsigned int remains()const;
	virtual unsigned int size() const;
	virtual const Type* array() const;
protected:
	virtual bool increaseCapacity(unsigned int newCapacity);
	virtual void ensureCapacity(unsigned int append);
protected:	
	unsigned int m_nCapacity;
	unsigned int m_nSize,m_nReadOffset;
	Type* m_buffer;
};


}//end of namespace bluemei