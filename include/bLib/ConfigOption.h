#pragma once
#include "bluemeiLib.h"
#include "BString.h"
#include "RuntimeException.h"

namespace bluemei{

/////////////////////////////////////////////////////////////////////
// class ConfigOption
class BLUEMEILIB_API ConfigOption : public Object
{
public:
	typedef enum {
		OPTION_STRING,
		OPTION_INT,
		OPTION_FLOAT,
		OPTION_BOOLEAN,
		OPTION_CHOICE,
		OPTION_MAX_COUNT
	}option_t;

	ConfigOption(option_t type, const String& name, const String& description)
		: m_type(type), m_name(name), m_description(description) {}

	static ConfigOption* clone(const ConfigOption& value);
public:
	ConfigOption::option_t type() const { return m_type; }
	String name() const { return m_name; }

	String description() const { return m_description; }
	void setDescription(const String& val) { m_description = val; }

	String deprecatedMessage() const { return m_deprecated; }
	ConfigOption& setDeprecated(const String& val);

	virtual bool valueEquals(const ConfigOption& other) const=0;
	virtual String toString() const;
protected:
	option_t m_type;
	String m_name;
	String m_description;
	String m_deprecated;
};

class BLUEMEILIB_API StringOption : public ConfigOption
{
public:
	const static option_t OPTION_TYPE = OPTION_STRING;

	StringOption(const String& name, const String& value,
		const String& description)
		: ConfigOption(OPTION_TYPE, name, description),
		m_value(value) {}

	String value() const { return m_value; }
	void setValue(const String& val) { m_value = val; }

	virtual Object* clone() const;
	virtual bool valueEquals(const ConfigOption& other) const;
	virtual String toString() const;
protected:
	String m_value;
};

class BLUEMEILIB_API IntOption : public ConfigOption
{
public:
	const static option_t OPTION_TYPE = OPTION_INT;

	IntOption(const String& name, int value,
		const String& description,
		int minVal=MININT, int maxVal=MAXINT)
		: ConfigOption(OPTION_TYPE, name, description),
		m_value(value), m_min(minVal), m_max(maxVal){}

	int value() const { return m_value; }
	void setValue(int val);
	
	int minValue() const { return m_min; }
	void setMinValue(int val) { m_min = val; }

	int maxValue() const { return m_max; }
	void setMaxValue(int val) { m_max = val; }

	virtual Object* clone() const;
	virtual bool valueEquals(const ConfigOption& other) const;
	virtual String toString() const;
protected:
	int m_value;
	int m_min, m_max;
};

class BLUEMEILIB_API FloatOption : public ConfigOption
{
public:
	const static option_t OPTION_TYPE = OPTION_FLOAT;

	FloatOption(const String& name, double value,
		const String& description,
		double minVal=DBL_MIN, double maxVal=DBL_MAX)
		: ConfigOption(OPTION_TYPE, name, description),
		m_value(value), m_min(minVal), m_max(maxVal){}

	double value() const { return m_value; }
	void setValue(double val);
	
	double minValue() const { return m_min; }
	void setMinValue(double val) { m_min = val; }

	double maxValue() const { return m_max; }
	void setMaxValue(double val) { m_max = val; }

	virtual Object* clone() const;
	virtual bool valueEquals(const ConfigOption& other) const;
	virtual String toString() const;
protected:
	double m_value;
	double m_min, m_max;
};

class BLUEMEILIB_API BooleanOption : public ConfigOption
{
public:
	const static option_t OPTION_TYPE = OPTION_BOOLEAN;

	BooleanOption(const String& name, bool value, const String& description)
		: ConfigOption(OPTION_TYPE, name, description), m_value(value) {}

	bool value() const { return m_value; }
	void setValue(bool val) { m_value = val; }

	virtual Object* clone() const;
	virtual bool valueEquals(const ConfigOption& other) const;
	virtual String toString() const;
protected:
	bool m_value;
};

class BLUEMEILIB_API ChoiceOption : public ConfigOption
{
public:
	const static option_t OPTION_TYPE = OPTION_CHOICE;

	ChoiceOption(const String& name, const ConfigOption& value,
		const String& description);
	ChoiceOption(const String& name, const String& value,
		const String& description);
	ChoiceOption(const String& name, int value, const String& description);
	ChoiceOption(const String& name, double value, const String& description);
	ChoiceOption(const String& name, bool value, const String& description);
	virtual ~ChoiceOption();

	ConfigOption& value() const;
	void setValue(const ConfigOption& val);

	ChoiceOption& addChoice(const ConfigOption& val);
	ChoiceOption& addChoice(const String& val);
	ChoiceOption& addChoice(int val);
	ChoiceOption& addChoice(double val);
	ChoiceOption& addChoice(bool val);

	bool checkChoice(const ConfigOption& val);

	virtual Object* clone() const;
	virtual bool valueEquals(const ConfigOption& other) const;
	virtual String toString() const;
	virtual String choicesString() const;	
protected:
	static String option2String(const ConfigOption& option);
protected:
	ConfigOption* m_value;
	List<ConfigOption*> m_choices;
};

/////////////////////////////////////////////////////////////////////
// class ConfigGroup
class BLUEMEILIB_API ConfigGroup : public Object
{
public:
	ConfigGroup(const String& name="") : m_group(name) {}
	virtual ~ConfigGroup() {}

	void setName(const String& name) { m_group = name; }
	String name() const { return m_group; }
public:
	template <typename T>
	T option(const String& key) const {
		throw RuntimeException("Not implemented template<T> option()");
	}

	String operator[] (const String& key) const {
		return option<String>(key);
	}

	template <>
	String option(const String& key) const;
	
	template <>
	int option(const String& key) const;

	template <>
	bool option(const String& key) const;

	template <>
	double option(const String& key) const;

	/*virtual String option(const String& key, const String& default) const;
	virtual int option(const String& key, int default) const;
	virtual bool option(const String& key, bool default) const;
	virtual double option(const String& key, double default) const;*/
public:
	virtual bool exists(const String& key) const;
	virtual bool get(const String& key, ConfigOption*& value) const;
	virtual void set(const String& key, const ConfigOption& value);
	virtual bool update(const String& key, const String& value);

	virtual void registerOption(const ConfigOption& value);
	virtual String toString() const;
protected:
	// group name
	String m_group;
	// options map
	Map<String, ConfigOption*> m_propertiesMap;
};

/////////////////////////////////////////////////////////////////////
// class Config
class BLUEMEILIB_API Config : public Object
{
public:
	virtual ConfigGroup& group(const String& name, bool create=false) {
		if (!create && m_groups.count(name) == 0)
			throwpe(NotFoundException("No group named: " + name));
		return m_groups[name];
	}
	
	virtual ConfigGroup& operator[] (const String& name) {
		return this->group(name);
	}

	virtual const ConfigGroup& operator[] (const String& name) const {
		return const_cast<Config*>(this)->group(name);
	}

	virtual ConfigGroup& registerGroup(const ConfigGroup& value);
	virtual String name() const=0;
	virtual String toString() const;
private:
	// group -> {key: value, ...}
	Map<String, ConfigGroup> m_groups;
};

/////////////////////////////////////////////////////////////////////
// class ConfigException
class ConfigException : public Exception
{
public:
	ConfigException(const String& msg) : Exception(msg) {}
	virtual String name() const { return "ConfigException"; }
};

}//end of namespace bluemei