#ifndef File_H_H
#define File_H_H

#include "bluemeiLib.h"
#include "IOException.h"
#include "Object.h"

namespace bluemei{

#define BUFFER_SIZE 1024
#define EOL "\n"
#define PATH_NULL ""

class BLUEMEILIB_API File : public Object
{
public:
	File(size_t bufferSize);
	//File(string path,string openMode);
	File(String path=PATH_NULL,String openMode="wb+",size_t bufferSize=BUFFER_SIZE);
	virtual ~File();
private:
	void init(String path,String openMode,size_t bufferSize);
public:
	virtual void openFile(String path,String openMode);
	virtual String filePath() const;

	virtual size_t writeBytes(const char buf[],size_t length);
	virtual void writeLine(cstring value);
	virtual void writeLine(const String& value);
	virtual void writeInt(int value);

	size_t write(const byte* buffer,size_t length);//原始函数

	virtual size_t readBytes(char buf[],size_t length);
	virtual size_t readLine(String& line);
	virtual size_t readLine(std::string& line);
	virtual size_t readAll(char buf[],size_t bufSize);
	virtual int readInt();

	virtual size_t currentPos()const;
	virtual void setPos(size_t pos);
	virtual void seek(long offset);//相对现在的位置进行偏移offset
	virtual size_t getSize();

	virtual void flush();
	virtual void close();
private:
	String m_strPath;
	FILE *m_pFile;
	size_t m_nBufferSize,m_nUsedBufLength;
	char *m_buffer;
	bool m_bCloseAble;
};


}//end of namespace bluemei
#endif