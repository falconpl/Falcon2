/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logstream.cpp

  Log Listener writing data to a stream
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 24 Feb 2019 14:53:12 +0000
  Touch : Thu, 28 Feb 2019 22:43:30 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/logstream.h>

#include <iostream>

namespace Falcon {

void LogStreamListener::onMessage( const Falcon::LogSystem::Message& msg )
{
	auto now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	tm local_tm = *localtime(&tt);

	// This method is not really meant to be used by multiple threads,
	// but this lock prevents changing the underlying stream mid-output.
	std::lock_guard<std::mutex> guard(m_mtxStream);
	m_out << local_tm.tm_year +1900 << '-'
				<< local_tm.tm_mon + 1 << '-'
				<< local_tm.tm_mday << ' '
				<< local_tm.tm_hour << ':'
				<< local_tm.tm_min  << ':'
				<< local_tm.tm_sec  << ' ';

	m_out << msg.m_file << ':' << msg.m_line
		<< " [" << LogSystem::levelToString(msg.m_level) << "]"
		<< "(" << msg.m_category << ") " << msg.m_message << "\n";
}

}
/* end of logstream.cpp */
