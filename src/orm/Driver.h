#pragma once
#include "blib.h"


namespace brpc{


typedef struct ByteArray{
	byte* data;
	int length;
	ByteArray(){
		length=0;
		data=nullptr;
	}
	ByteArray(int len){
		length=len;
		data=new byte[len];
	}
	ByteArray(byte* d,int len=-1){
		if(len<0)
			len=strlen((cstring)d);
		length=len;
		data=new byte[length];
		memcpy(data,d,length);
	}

	void release()
	{
		if(length>0 || data!=nullptr)
		{
			delete[] data;
			length=0;
			data=nullptr;
		}
	}
}byte_array_t;


typedef class ResultSet : public Object
{
public:
	virtual bool next() = 0;
	virtual bool previous() = 0;

	virtual bool absolute(uint64 row) = 0;
	virtual void afterLast() = 0;
	virtual void beforeFirst() = 0;

	virtual uint32 fieldsCount() const = 0;
	virtual uint64 rowsCount() const = 0;
	virtual uint32 columnIndex(cstring columnName) const = 0;

	virtual void close() = 0;
	virtual bool isClosed() const = 0;

	virtual cstring getString(uint32 columnIndex) = 0;
	virtual cstring getString(cstring columnName) = 0;

	virtual int getInt(uint32 columnIndex) = 0;
	virtual int getInt(cstring columnName) = 0;

	virtual double getDouble(uint32 columnIndex) = 0;
	virtual double getDouble(cstring columnName) = 0;

	virtual ByteArray getBytes(uint32 columnIndex) = 0;
	virtual ByteArray getBytes(cstring columnName) = 0;

	virtual Object* getObject(uint32 columnIndex) = 0;
	virtual Object* getObject(cstring columnName) = 0;

	virtual size_t getDataLength(uint32 columnIndex) = 0;
	virtual size_t getDataLength(cstring columnName) = 0;
} DatabaseResult;


typedef class DriverConnection : public Object
{
public:
	static DriverConnection* loadDriver(const String& backend);
public:
	virtual void open(const String& connInfo) = 0;
	virtual void close() = 0;

	virtual ResultSet* executeQuery(cstring sql, size_t len=0) = 0;
	virtual int executeUpdate(cstring sql, size_t len=0) = 0;
} DatabaseDriverConnection;


}//end of namespace brpc