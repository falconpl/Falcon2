/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logstream.cpp

  Log Listener writing data to a stream
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 24 Feb 2019 14:53:12 +0000
  Touch : Sat, 02 Mar 2019 11:12:05 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/logstream.h>

#include <iostream>
#include <iomanip>

namespace falcon {

void LogStreamListener::onMessage( const falcon::LogSystem::Message& msg )
{
	auto now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	tm local_tm = *localtime(&tt);

	size_t fpos = msg.m_file.rfind(FALCON_DIR_SEP_CHR);
	std::string file;
	if (fpos != std::string::npos) {
		file = msg.m_file.substr(fpos+1);
	}
	else {
		file =  msg.m_file;
	}

	// This method is not really meant to be used by multiple threads,
	// but this lock prevents changing the underlying stream mid-output.
	std::lock_guard<std::mutex> guard(m_mtxStream);
	std::ostream& out = *m_pout;

	// TODO: use a format to print
	out
		<< local_tm.tm_year +1900 << '-'
		<< std::setfill('0') << std::setw(2)
		<< local_tm.tm_mon + 1 << '-'
		<< std::setfill('0') << std::setw(2)
		<< local_tm.tm_mday << ' '
		<< std::setfill('0') << std::setw(2)
		<< local_tm.tm_hour << ':'
		<< std::setfill('0') << std::setw(2)
		<< local_tm.tm_min  << ':'
		<< std::setfill('0') << std::setw(2)
		<< local_tm.tm_sec  << ' '
		<< std::setw(0);

	out	<< "[" << LogSystem::levelToString(msg.m_level) << "] ";
	if(! msg.m_category.empty()) {
		out << "(" << msg.m_category << ") ";
	}
	out << file << ':' << msg.m_line;
	out << " " << msg.m_message << "\n";

	// TODO: add a flag to do this optionally
	out.flush();
}

}
/* end of logstream.cpp */
