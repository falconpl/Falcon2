/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logger.cpp

  Default Logger Implementation
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Fri, 01 Mar 2019 23:15:12 +0000
  Touch : Sat, 02 Mar 2019 11:12:05 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/logger.h>

namespace falcon {

thread_local std::string Logger::m_category{""};
thread_local std::string Logger::m_tempCategory{""};
thread_local std::ostringstream Logger::m_composer;
thread_local std::string Logger::m_msgFile{""};
thread_local int Logger::m_msgLine{0};
thread_local LOGLEVEL Logger::m_msgLevel{LLTRACE};

Logger::Logger()
{
	m_dflt = std::make_shared<LogStreamListener>();
	addListener(m_dflt);
}


void Logger::categoryFilter(const std::string& category, LOGLEVEL l) {
	if (m_proxy.get()) {
		m_proxy->detach();
	}
	m_proxyBaseLevel = level();
	if(l > m_proxyBaseLevel) {
		level(l);
	}
	m_proxy = std::make_shared<LogProxyListener>(m_dflt);
	m_proxy->category(category);
	m_proxy->level(l);
	addListener(m_proxy);
}


void Logger::clearFilter() {
	m_proxy->detach();
	level(m_proxyBaseLevel);
}

}

/* end of logger.cpp */
