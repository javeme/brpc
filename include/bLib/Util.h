#ifndef Util_H_H
#define Util_H_H
#include "bluemeiLib.h"
#include "Object.h"

namespace bluemei{

using std::string;
#define PropertiesMap map<string,string>

class BLUEMEILIB_API Util : public Object
{
public:
	Util(void);
	~Util(void);
public:
	static bool simpleEncode(string& plaintext,const string& code);//明文->密码
	static bool simpleDecode(string& cipher,const string& code);//密码->明文
public:
	static bool getSelfPath(string& path);
	static bool open(const string& name,const string& arg="");
public:
	static void replaceString(string & strToReplace, const string & strSrc, const string &strDst);
	static void trim(string &str);
	static short getChar(int& pos,const string& str);
	static int match(const string& value,const string& regex);//正则匹配

	static int str2Int(cstring str);
	static string int2Str(int i);

	static double str2Float(cstring str);
	static string float2Str(double f);

	static bool str2Boolean(cstring str);
	static string boolean2Str(bool b);

	//判断字符串是否为数字
	template<class CharType>
	static bool isIntNumber(const CharType* str,int len);
	//判断字符串是否为浮点数
	template<class CharType>
	static bool isFloatNumber(const CharType* str,int len);
public:
	static int random();

	static string uuid4();
	static string uuid1();
	static string guid();
};

template<class CharType>
bool Util::isIntNumber(const CharType* str,int len)
{
	//int len=strlen(str);
	int i=0;
	if(len>0 && str[0]=='-')//负号
		i++;
	for(;i<len;i++)
	{
		if(!isdigit(str[i]))
			return false;
	}
	return true;
}

template<class CharType>
bool Util::isFloatNumber(const CharType* str,int len)
{
	int dotCount=0;
	//int len=strlen(str);
	int i=0;
	if(len>0 && str[0]=='-')//负号
		i++;
	for(;i<len;i++)
	{
		auto c=str[i];
		if(c=='.'){
			dotCount++;
			if(dotCount>1)
				return false;
		}
		else if(!isdigit(c))
			return false;
	}
	return true;
}

}//end of namespace bluemei
#endif