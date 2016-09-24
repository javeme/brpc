#pragma once
#include "bluemeiLib.h"
#include "File.h"
#include "Object.h"

namespace bluemei{

using std::string;
#define PropertiesMap map<string,string>

class BLUEMEILIB_API SimpleCfgFile : public Object
{
public:
	SimpleCfgFile(const string& path);
	~SimpleCfgFile(void);
private:
	PropertiesMap m_propertiesMap;
	string m_filePath;	
	string m_content;
	bool m_isChanged;
public:
	void readPropertyFromFile(const string& path);
public:	
	bool getProperty(const string& key,string& value);//获取配置属性		
	bool setProperty(const string& key,const string& value);//设置配置属性
	bool removeProperty(const string& key);
	void saveProperty();
};

}//end of namespace bluemei