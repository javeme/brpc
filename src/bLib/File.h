#ifndef File_H_H
#define File_H_H

#include "bluemeiLib.h"
#include <iostream>
#include "IOException.h"
#include "Object.h"
 
namespace bluemei{

#define BUFFER_SIZE 1024
#define CRLF "\r\n"
#define PATH_NULL ""

class BLUEMEILIB_API File : public Object
{
public:
	File(unsigned long bufferSize);
	//File(string path,string openMode);
	File(String path=PATH_NULL,String openMode="wb+",unsigned long bufferSize=BUFFER_SIZE);
	virtual ~File();
private:
	void init(String path,String openMode,unsigned long bufferSize);
public:
	virtual void openFile(String path,String openMode);
	virtual String filePath() const;

	virtual unsigned long writeBytes(const char buf[],unsigned long length);
	virtual void writeLine(cstring value);
	virtual void writeLine(const String& value);
	virtual void writeInt(int value);

	unsigned int write(const byte* buffer,unsigned int length);//原始函数

	virtual unsigned int readBytes(char buf[],unsigned long length);
	virtual unsigned int readLine(String& line);
	virtual unsigned int readLine(std::string& line);
	virtual unsigned int readAll(char buf[],unsigned long bufSize);
	virtual int readInt();

	virtual unsigned int currentPos()const;
	virtual void setPos(unsigned int pos);
	virtual void seek(long offset);//相对现在的位置进行偏移offset
	virtual unsigned int getSize();

	virtual void flush();
	virtual void close();
private:
	String m_strPath;
	FILE *m_pFile;
	unsigned long m_nBufferSize,m_nUsedBufLength;
	char *buffer;
	bool m_bCloseAble;
};


}//end of namespace bluemei
#endif