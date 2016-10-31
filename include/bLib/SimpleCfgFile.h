#pragma once
#include "bluemeiLib.h"
#include "ConfigOption.h"
#include "File.h"

namespace blib{

// class SimpleCfgFile
class BLUEMEILIB_API SimpleCfgFile : public Config
{
public:
	SimpleCfgFile(const String& path="");
	~SimpleCfgFile(void);
public:
	virtual String path() const { return m_filePath; }
	virtual String name() const { return m_filePath; }
	virtual void parseFromFile(const String& path);
protected:
	void parse();
	ConfigGroup& setOptionToGroup(const String& group,
		const String& name, const String& value);
private:
	String m_filePath;
};

}//end of namespace blib