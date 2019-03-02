/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logger.h

  Default log singleton for Falcon applications
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Thu, 28 Feb 2019 21:04:31 +0000
  Touch : Sat, 02 Mar 2019 11:12:05 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_LOGGER_H_
#define _FALCON_LOGGER_H_

#include <falcon/logsystem.h>
#include <falcon/logstream.h>
#include <falcon/singleton.h>

/** Compile-time log filter */
#ifndef FALCON_MIN_LOG_LEVEL
#define FALCON_MIN_LOG_LEVEL ::Falcon::LLTRACE
#endif

namespace Falcon {

using LOGLEVEL = LogSystem::LEVEL;

constexpr auto LLDISABLE = -1;
constexpr auto LLCRIT = LogSystem::LEVEL::CRITICAL;
constexpr auto LLERR = LogSystem::LEVEL::ERROR;
constexpr auto LLWARN = LogSystem::LEVEL::WARN;
constexpr auto LLINFO = LogSystem::LEVEL::INFO;
constexpr auto LLDEBUG = LogSystem::LEVEL::DEBUG;
constexpr auto LLTRACE = LogSystem::LEVEL::TRACE;

class Logger: public LogSystem
{
public:

	Logger()
	{
		m_dflt = std::make_shared<LogStreamListener>();
		addListener(m_dflt);
	}

	static Logger& instance() {
		static Logger theLogger;
		return theLogger;
	}

	std::shared_ptr<LogStreamListener> defaultListener() const {
		return m_dflt;
	}

	void setCategory(const std::string& category) noexcept {
		m_category = category;
	}

	const std::string& getCategory() const noexcept {
		return m_category;
	}

	void commit()
	{
		log(m_msgFile, m_msgLine, m_msgLevel, m_category, m_composer.str());
		readyStream();
	}

	void setLevel(LOGLEVEL lvl) {
		m_msgLevel = lvl;
	}

	void setFile(const std::string& file) {
		m_msgFile = file;
	}

	void setLine(int line) {
		m_msgLine = line;
	}

	class AutoEnd
	{
	private:
	    Logger* m_obj;

	public:
	    explicit AutoEnd (Logger& obj, const std::string& file, int line, LOGLEVEL lvl):
			m_obj(nullptr)
	    {
	    	if(FALCON_MIN_LOG_LEVEL >= lvl && obj.level() >= lvl) {
	    		obj.setFile(file);
	    		obj.setLine(line);
	    		obj.setLevel(lvl);
	    		m_obj = &obj;
	    	}
	    }

	    ~AutoEnd () {if(m_obj){m_obj->commit();}}
	    bool doLog() const noexcept {return m_obj != nullptr;}
	    Logger& obj () const noexcept {return *m_obj;}
	};

	class BlockEnd
	{
	private:
	    Logger* m_obj;

	public:
	    explicit BlockEnd (Logger& obj, const std::string& file, int line, LOGLEVEL lvl):
			m_obj(nullptr)
	    {
	    	if(FALCON_MIN_LOG_LEVEL >= lvl && obj.level() >= lvl) {
	    		obj.setFile(file);
	    		obj.setLine(line);
	    		obj.setLevel(lvl);
	    		m_obj = &obj;
	    	}
	    }
	    void complete() {m_obj->commit(); m_obj = nullptr;}
	    operator bool() const noexcept {return m_obj != nullptr;}
	};

	// Hearth of the composition.
	template<typename T>
	const Logger& operator<<(const T& v) const {
		m_composer << v;
		return *this;
	}

private:
	std::shared_ptr<LogStreamListener> m_dflt;
	static thread_local std::ostringstream m_composer;
	static thread_local std::string m_category;
	static thread_local std::string m_msgFile;
	static thread_local int m_msgLine;
	static thread_local LOGLEVEL m_msgLevel;

	void readyStream() noexcept {
		m_composer.clear();
		m_composer.str("");
	}
};

template <typename T>
const Logger::AutoEnd& operator << (const Logger::AutoEnd& ae, T&& arg)
{
	if (ae.doLog()) {
		ae.obj() << std::forward<T>(arg);
	}
    return ae;
}

#define LOGGER (::Falcon::Logger::instance())
#define LOG_CATEGORY(cat) (LOGGER.setCategory(cat))

#define LOG(lvl) (::Falcon::Logger::AutoEnd(LOGGER, __FILE__, __LINE__, lvl))
#define LOG_CRIT LOG(::Falcon::LLCRIT)
#define LOG_ERR  LOG(::Falcon::LLERR)
#define LOG_WARN LOG(::Falcon::LLWARN)
#define LOG_INFO LOG(::Falcon::LLINFO)
#define LOG_DBG  LOG(::Falcon::LLDEBUG)
#define LOG_TRC  LOG(::Falcon::LLTRACE)

#define LOG_BLOCK(lvl) \
	for( ::Falcon::Logger::BlockEnd __ender(LOGGER, __FILE__, __LINE__, lvl); \
		FALCON_MIN_LOG_LEVEL >= lvl && LOGGER.level() >= lvl && __ender; \
		__ender.complete() )

#define LOG_BLOCK_CRIT LOG_BLOCK(::Falcon::LLCRIT)
#define LOG_BLOCK_ERR  LOG_BLOCK(::Falcon::LLERR)
#define LOG_BLOCK_WARN LOG_BLOCK(::Falcon::LLWARN)
#define LOG_BLOCK_INFO LOG_BLOCK(::Falcon::LLINFO)
#define LOG_BLOCK_DBG  LOG_BLOCK(::Falcon::LLDEBUG)
#define LOG_BLOCK_TRC  LOG_BLOCK(::Falcon::LLTRACE)

}

#endif /* _FALCON_LOGGER_H_ */

/* end of logger.h */

