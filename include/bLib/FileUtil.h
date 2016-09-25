#ifndef FileUtil_H_H
#define FileUtil_H_H

#include "bluemeiLib.h"
#include "BString.h"
#include "ArrayList.h"
#include "CodeUtil.h"

namespace bluemei{

#define PATH_SEPARATOR "/"

typedef struct FileInfo
{
	String name;
	dword  mode;
	int64  size;
	time_t createTime;
	time_t accessTime;
	time_t updateTime;
	time_t statChangeTime;
}file_t;

class BLUEMEILIB_API FileUtil : public Object
{
public:
	static bool exists(const String& path);
	static bool readable(const String& path);
	static bool writable(const String& path);
	static bool executable(const String& path);

	static bool isfile(const String& path);
	static bool isdir(const String& path);

	static ArrayList<String> list(const String& path);
	static bool remove(const String& path);
	static bool rename(const String& from, const String& to);
	static bool chmod(const String& path, int mode);

	static FileInfo info(const String& path);

	static bool mkdir(const String& dir);
	static bool mkdirs(const String& dir);

	static String getcwd();
	static bool chdir(const String& path);
};

}//end of namespace bluemei
#endif