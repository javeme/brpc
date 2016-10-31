#ifndef _Log_h_
#define _Log_h_

#include "File.h"
#include "HashMap.h"
#include "Looper.h"

namespace blib{

class Template;

/*
* 日志类
* @author Javeme
* @create 2014/7/4
*/
class BLUEMEILIB_API Log : public Object
{
public:
	enum LogLevel
	{
		LOG_UNKNOWN = 0,
		LOG_FATAL = 1,   /// A fatal error. The application will most likely terminate. This is the highest priority.
		LOG_CRITICAL,	/// A critical error. The application might not be able to continue running successfully.
		LOG_ERROR,	   /// An error. An operation did not complete successfully, but the application as a whole is not affected.
		LOG_WARNING,	 /// A warning. An operation completed with an unexpected result.
		LOG_NOTICE,	  /// A notice, which is an information with just a higher priority.
		LOG_INFO,		/// An informational message, usually denoting the successful completion of an operation.
		LOG_DEBUG,	   /// A debugging message.
		LOG_TRACE,	   /// A tracing message. This is the lowest priority.
		LOG_LEVEL_SIZE
	};
	typedef HashMap<String, String> LogCtx;

	Log(const String& name, const String& path, LogLevel level=LOG_INFO);
	virtual ~Log();
public:
	static Log* getLogger(const String& name="");
public:
	virtual void log(LogLevel level, const String& msg, const LogCtx& ctx=s_nullCtx);

	void fatal(const String& str, const LogCtx& ctx=s_nullCtx)
	{
		return log(LOG_FATAL, str, ctx);
	}
	void critical(const String& str, const LogCtx& ctx=s_nullCtx)
	{
		return log(LOG_CRITICAL, str, ctx);
	}
	void error(const String& str, const LogCtx& ctx=s_nullCtx)
	{
		return log(LOG_ERROR, str, ctx);
	}
	void warn(const String& str, const LogCtx& ctx=s_nullCtx)
	{
		return log(LOG_WARNING, str, ctx);
	}
	void notice(const String& str, const LogCtx& ctx=s_nullCtx)
	{
		return log(LOG_NOTICE, str, ctx);
	}
	void info(const String& str, const LogCtx& ctx=s_nullCtx)
	{
		return log(LOG_INFO, str, ctx);
	}
	void debug(const String& str, const LogCtx& ctx=s_nullCtx)
	{
		return log(LOG_DEBUG, str, ctx);
	}
	void trace(const String& str, const LogCtx& ctx=s_nullCtx)
	{
		return log(LOG_TRACE, str, ctx);
	}

	String name() const { return m_name; }

	LogLevel getLevel() const { return m_level; }
	void setLevel(LogLevel val) { m_level = val; }

	void setLevel(const String& val);
	String getLevelDscr() const;

	void updateFormatter(const String& val);
public:
	static String level2String(LogLevel val);
	static LogLevel string2Level(const String& val);
protected:
	void printLine(const String& str);
	void print(const String& str);

	String format(const LogCtx& ctx);
protected:
	String m_name;
	String m_path;
	LogLevel m_level;
	String m_formatter;
private:
	File m_file;
	Looper m_looper;
	Template* m_tmpl;
protected:
	const static LogCtx s_nullCtx;
};

/*
* 日志实例管理器
* @author Javeme
* @create 2014/7/4
*/
class BLUEMEILIB_API LogManager : public Object
{
public:
	static LogManager& instance();
	static void destroy();
private:
	LogManager();
	virtual ~LogManager();
public:
	Log* getLogger(const String& name="");
	Log* initLogger(const String& name, const String& path,
		Log::LogLevel level=Log::LOG_INFO);
	void destroyLoggers();

	String getDefault() const { return m_default; }
	void setDefault(const String& val) { m_default = val; }
protected:
	HashMap<String, Log*> m_loggers;
	String m_default;
private:
	static LogManager* s_manager;
};

}//end of namespace blib
#endif