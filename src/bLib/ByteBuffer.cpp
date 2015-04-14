#include "ByteBuffer.h"

namespace bluemei{


ByteBuffer::ByteBuffer(unsigned int maxSize,unsigned int size):DataBuffer(maxSize,size)
{
	m_bBigEndian=false;
}

ByteBuffer::~ByteBuffer(void)
{
}

ByteBuffer::ByteBuffer(ByteBuffer&& other)
{
	*this = std::move(other);
}

ByteBuffer& ByteBuffer::operator=(ByteBuffer&& other)
{
	if(this!=&other)
	{
		this->m_bBigEndian=other.m_bBigEndian;
		DataBuffer<byte>::operator=(std::move(other));
	}
	return *this;
}

bool ByteBuffer::isBigEndian() const
{
	return m_bBigEndian;
}

void ByteBuffer::setBigEndian( bool val )
{
	m_bBigEndian = val;
}

unsigned int ByteBuffer::writeString( const String& str )
{
	int len=str.length();
	unsigned int offset=writeShort(len);
	write((byte*)str.c_str(),len);
	return offset;
}

String ByteBuffer::readString()
{
	int len=readShort();
	byte* buf=new byte[len];
	try
	{
		read(buf,len);
	}catch(...)
	{
		delete[]buf;
		throw;
	}
	String str((char*)buf,len);
	delete[]buf;
	return str;
}


}//end of namespace bluemei