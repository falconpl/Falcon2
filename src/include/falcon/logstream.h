/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logstream.h

  Log Listener writing data to a stream
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 24 Feb 2019 14:44:02 +0000
  Touch : Thu, 28 Feb 2019 22:43:30 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_LOGSTREAM_H_
#define _FALCON_LOGSTREAM_H_

#include "logsystem.h"
#include <streambuf>

namespace Falcon {

class FALCON_API_ LogStreamListener: public LogSystem::Listener
{
private:

	class NullBuffer : public std::streambuf
	{
	public:
	  int overflow(int c) override { return c; }
	};

public:

	LogStreamListener() noexcept:
		m_out(&m_nullBuffer)
	{}

	LogStreamListener(std::ostream& out) noexcept:
		m_out(out.rdbuf())
	{}

	LogStreamListener(const LogStreamListener& other) noexcept:
			m_out(other.m_out.rdbuf())
	{}

	LogStreamListener(LogStreamListener& other) noexcept:
			m_out(std::move(other.m_out.rdbuf()))
	{}

	~LogStreamListener() = default;

	LogStreamListener& operator = (const LogStreamListener& other) noexcept {
		std::lock_guard<std::mutex> guard(m_mtxStream);
		m_out.rdbuf(other.m_out.rdbuf());
		return *this;
	}

	void setStream(std::ostream& out) noexcept {
		std::lock_guard<std::mutex> guard(m_mtxStream);
		m_out.rdbuf(out.rdbuf());
	}

protected:
    virtual void onMessage( const Falcon::LogSystem::Message& msg ) override;

private:
    NullBuffer m_nullBuffer;
    std::ostream m_out;
    std::mutex m_mtxStream;
};

}

#endif /* _FALCON_LOGSTREAM_H_ */

/* end of logstream.h */

