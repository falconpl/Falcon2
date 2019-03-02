/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logger.h

  Default log singleton for Falcon applications
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Thu, 28 Feb 2019 21:04:31 +0000
  Touch : Sat, 02 Mar 2019 01:33:45 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_LOGGER_H_
#define _FALCON_LOGGER_H_

#include <falcon/logsystem.h>
#include <falcon/logstream.h>
#include <falcon/singleton.h>

namespace Falcon {

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

private:
	std::shared_ptr<LogStreamListener> m_dflt;
	static thread_local std::string m_category;
};


#define LOG (Falcon::Logger::instance())

constexpr auto CRITICAL = LogSystem::LEVEL::CRITICAL;
constexpr auto ERROR = LogSystem::LEVEL::CRITICAL;
constexpr auto WARN = LogSystem::LEVEL::CRITICAL;
constexpr auto INFO = LogSystem::LEVEL::CRITICAL;
constexpr auto DEBUG = LogSystem::LEVEL::CRITICAL;
constexpr auto TRACE = LogSystem::LEVEL::CRITICAL;

Logger& operator <<(Logger& out, const std::string& str) {
	out.log("a file", 100, CRITICAL, out.getCategory(), str);
	return out;
}

}

#endif /* _FALCON_LOGGER_H_ */

/* end of logger.h */

