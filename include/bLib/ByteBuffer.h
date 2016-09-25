#pragma once
#include "DataBuffer.cpp"
#include "CodeUtil.h"

namespace bluemei{

class BLUEMEILIB_API ByteBuffer : public DataBuffer<byte>
{
public:
	ByteBuffer(unsigned int maxSize=8,unsigned int size=0);
	virtual ~ByteBuffer(void);
public:
	ByteBuffer(ByteBuffer&& other);
	ByteBuffer& operator=(ByteBuffer&& other);
protected:
	bool m_bBigEndian;
public:
	bool isBigEndian() const;
	void setBigEndian(bool val);
public:
	virtual unsigned int write(const byte buf[],unsigned int len){
		return put(buf,len);
	}

	virtual unsigned int write(byte b){
		return put(b);
	}

	virtual unsigned int writeBytes(const byte buf[],unsigned int len){ return write(buf,len); }
	virtual unsigned int writeByte(byte b){ return write(b); }

	virtual unsigned int writeBoolean(bool b){
		return write(b);
	}

	virtual unsigned int writeChar(char c){
		return write(c);
	}

	virtual unsigned int writeShort(short i){
		const static unsigned int size=sizeof(i);
		byte buf[size];
		//CodeUtil::shortToBytes((char*)buf,i);
		CodeUtil::bigIntToBytes(buf,i,m_bBigEndian);
		return write(buf,size);
	}

	virtual unsigned int writeInt(int i){
		const static unsigned int size=sizeof(i);
		byte buf[size];
		CodeUtil::bigIntToBytes(buf,i,m_bBigEndian);
		return write(buf,size);
	}

	virtual unsigned int writeFloat(float f){
		const static unsigned int size=sizeof(f);
		byte buf[size];
		CodeUtil::floatToBytes(buf,f);
		return write(buf,size);
	}

	virtual unsigned int writeDouble(double f){
		const static unsigned int size=sizeof(f);
		byte buf[size];
		CodeUtil::doubleToBytes(buf,f);
		return write(buf,size);
	}

	virtual unsigned int writeString(const String& str);


	virtual void readBytes(byte buf[],unsigned int len){ return read(buf,len); }
	virtual byte readByte(){ return read(); }

	virtual bool readBoolean(){
		return read()!=0;
	}

	virtual char readChar(){
		return read();
	}

	virtual short readShort(){
		short i=0;
		const static unsigned int size=sizeof(i);
		byte buf[size];
		read(buf,size);
		i=CodeUtil::bytesToBigInt<short>(buf,m_bBigEndian);
		return i;
	}

	virtual int readInt(){
		int i=0;
		const static unsigned int size=sizeof(i);
		byte buf[size];
		read(buf,size);
		i=CodeUtil::bytesToBigInt<int>(buf,m_bBigEndian);
		return i;
	}

	virtual float readFloat(){
		float f=0;
		const static unsigned int size=sizeof(f);
		byte buf[size];
		read(buf,size);
		f=CodeUtil::bytesToFloat(buf);
		return f;
	}

	virtual double readDouble(){
		double f=0;
		const static unsigned int size=sizeof(f);
		byte buf[size];
		read(buf,size);
		f=CodeUtil::bytesToDouble(buf);
		return f;
	}
	virtual String readString();
};


}//end of namespace bluemei