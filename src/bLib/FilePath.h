#ifndef FilePath_H_H
#define FilePath_H_H

#include "bluemeiLib.h"
#include "StringBuilder.h"
 
namespace bluemei{

#define PATH_SEPARATOR "/"
	
class BLUEMEILIB_API FilePath : public Object
{
public:
	FilePath(const String& path="");
	virtual ~FilePath();
public:
	FilePath& append(const String& path);

	String dirName() const;//directory
	String fileName() const;//file name

	String toString() const;
	operator String() const { return toString(); }
protected:
	StringBuilder m_path;
};

class BLUEMEILIB_API FileUtil : public Object
{
public:
	static bool exists(const String& path);
	static bool mkdir(const String& dir);
	static bool mkdirs(const String& dir);
};

}//end of namespace bluemei
#endif