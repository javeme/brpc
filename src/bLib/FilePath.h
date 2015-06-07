#ifndef FilePath_H_H
#define FilePath_H_H

#include "bluemeiLib.h"
#include "StringBuilder.h"
#include "FileUtil.h"

namespace bluemei{

class BLUEMEILIB_API FilePath : public Object
{
public:
	FilePath(const String& path="");
	virtual ~FilePath();
public:
	bool operator == (const FilePath& other) const {
		return m_path == other.m_path;
	}
	operator String() const { return toString(); }
public:
	FilePath& append(const String& path);

	String dirName() const;//directory
	String fileName() const;//file name

	String toString() const;
public:
	bool exists() const;
	bool readable() const;
	bool writable() const;
	bool executable() const;

	bool isfile() const;
	bool isdir() const;

	ArrayList<FilePath> list() const;
protected:
	StringBuilder m_path;
};

}//end of namespace bluemei
#endif