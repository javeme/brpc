#ifndef String_H_H
#define String_H_H
#include "bluemeiLib.h"
#include "Object.h"
#include "ClassField.h"

namespace bluemei{

template<class T> class ArrayList;
#define TRIM_LEFT 1
#define TRIM_RIGHT 2
#define TRIM_BOTH 0

#define STR_SMALL_SIZE 8

/*
* 字符串类
* @author Javeme(整理)
* @create 2012/4/15
* 初始源码来自http://www.programfan.com/article/2917.html
*/
class BLUEMEILIB_API String : public Object
{
public:
	// constructors
	String();
	String(cstring src, unsigned int len=-1);
	String(const std::string &src);
	String(const String &src);
	String(String&& src); // move
	virtual ~String();

	// operators
	String& operator= (cstring src);
	String& operator= (const String &src);
	String& operator= (String&& src); // move
	bool operator== (const String &other) const{ return compare(other); }
	bool operator!= (const String &other) const{ return !compare(other); }
	bool operator< (cstring str) const;
	bool operator== (cstring str) const{ return compare(str); };
	bool operator!= (cstring str) const{ return !compare(str); };
	String operator+ (const String &add) const;
	String& operator+= (const String &add);

	// conversions
	operator cstring() const;
	operator std::string() const;

	// member methods
	virtual String& append(const String &add);
	virtual char charAt(int index) const;
	virtual unsigned int length() const;
	virtual bool empty() const{ return length() == 0; }
	virtual int find(const String& substr, unsigned int start=0) const;
	virtual int rfind(const String& substr, unsigned int fromIndex=-1) const;
	virtual bool contain(const String& substr) const;
	virtual bool startWith(const String& substr) const;
	virtual bool endWith(const String& substr) const;
	virtual String trim(int part=TRIM_BOTH) const;
	virtual String replace(const String& strNeedReplaced, const String& strReplace) const;
	virtual ArrayList<String> splitWith(const String& separator, unsigned int max=-1) const;
	virtual String join(const ArrayList<String>& list, unsigned int from=0, unsigned int to=-1) const;
	virtual String substring(unsigned int start ,unsigned int sublen) const;
	virtual String substring(unsigned int start) const;
	virtual String getLeftSub(unsigned int sublen) const;
	virtual String getRightSub(unsigned int sublen) const;
	virtual bool compare(const String & other, bool caseSensitive=true) const;
	virtual bool compare(cstring other, bool caseSensitive=true) const;
	virtual String toUpper() const;
	virtual String toLower() const;

	virtual String toString() const;
	virtual cstring c_str() const;
	static String format(cstring frmt, ...);
private:
	// union to store string data
	typedef union {
		// small string optimization: memory may be in stack
		char buf[STR_SMALL_SIZE]; // and this permit aliasing
		// memory in heap
		char *ptr;
	} str_buf_t;

	// init memory
	void init(unsigned int len);
	// destroy memory
	void destroy();
	// resize memory
	void resize(unsigned int len);
	// update memory
	void steal(String& src);
	// get data ptr
	char* data() const;
	// check offset is out of bound
	void checkBound(unsigned int offset) const;
private:
	str_buf_t m_chars;
	unsigned int m_nLength;
	unsigned int m_nSize;
};


#define APPEND2STRING(Type)\
	BLUEMEILIB_API String operator+(const Type& str1,const String& str2);\
	BLUEMEILIB_API String operator+(const String& str1,const Type& str2);

APPEND2STRING(bool);
APPEND2STRING(char);
APPEND2STRING(short);
APPEND2STRING(int);
APPEND2STRING(long);
APPEND2STRING(long long);
APPEND2STRING(float);
APPEND2STRING(double);
APPEND2STRING(cstring);

template <class Type>
class BLUEMEILIB_TEMPLATE Value2String
{
protected:
	const Type& value;
public:
	Value2String(const Type& v):value(v){}
	operator String() const{
		const bool convertible = is_convertible<Type, Object*>::value;
		if(convertible)
			return dynamic_caster<Type, convertible>::toObject(value)->toString();
		return "unknown";
	}
};

template<> Value2String<cstring>::operator String() const{ return String(value); }
template class BLUEMEILIB_API Value2String<cstring>;

template<> Value2String<String>::operator String() const{ return value; }
template class BLUEMEILIB_API Value2String<String>;

template<> Value2String<bool>::operator String() const{ return value?"true":"false"; }
template class BLUEMEILIB_API Value2String<bool>;

template<> Value2String<char>::operator String() const{ return String::format("%c",value); }
template class BLUEMEILIB_API Value2String<char>;

template<> Value2String<short>::operator String() const{ return String::format("%d",value); }
template class BLUEMEILIB_API Value2String<short>;

template<> Value2String<int>::operator String() const{ return String::format("%d",value); }
template class BLUEMEILIB_API Value2String<int>;

template<> Value2String<long>::operator String() const{ return String::format("%ld",value); }
template class BLUEMEILIB_API Value2String<long>;

template<> Value2String<long long>::operator String() const{ return String::format("%ld",value); }
template class BLUEMEILIB_API Value2String<long long>;

template<> Value2String<float>::operator String() const{ return String::format("%f",value); }
template class BLUEMEILIB_API Value2String<float>;

template<> Value2String<double>::operator String() const{ return String::format("%lf",value); }
template class BLUEMEILIB_API Value2String<double>;

template<> Value2String<const FieldInfo*>::operator String() const{
	return String::format("%s<type %s>", value->name(), value->typeInfo().name());
}
template class BLUEMEILIB_API Value2String<const FieldInfo*>;

/*
template <class Type>
class Value2String
{
protected:
	const Type& value;
public:
	Value2String(const Type& v):value(v){}
	operator String() const{
		return "unknown";
	}
};
template <>
Value2String<int>::operator String() const{ return String::format("%d",value); }

template <class Type>
String operator+(const Type& str1,const String& str2)
{
	return ((String)Value2String<Type>(str1))+str2;
}
template <class Type>
String operator+(const String& str1,const Type& str2)
{
	return str1+((String)Value2String<Type>(str2));
}


#define APPEND2STRING(Type)\
template <>\
BLUEMEILIB_API String operator+(const Type& str1,const String& str2);\
template <>\
BLUEMEILIB_API String operator+(const String& str2,const Type& str1);

APPEND2STRING(char);
APPEND2STRING(short);
APPEND2STRING(int);
APPEND2STRING(long);
APPEND2STRING(long long);
APPEND2STRING(float);
APPEND2STRING(double);*/

}//end of namespace bluemei
#endif