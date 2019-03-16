/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logstream.h

  Log Listener writing data to a stream
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 24 Feb 2019 14:44:02 +0000
  Touch : Sat, 02 Mar 2019 01:33:45 +0000

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

	LogStreamListener() noexcept
	{}
	LogStreamListener(const LogStreamListener& other)=delete;
	LogStreamListener(LogStreamListener& other)=delete;
	~LogStreamListener() = default;
	LogStreamListener& operator = (const LogStreamListener& other) = delete;

	/** Set the stream by passing the ownership. */
	void own(std::unique_ptr<std::ostream>& out) noexcept {
		std::lock_guard<std::mutex> guard(m_mtxStream);
		m_uptrStream = std::move(out);
		m_pout = m_uptrStream.get();
	}

	/** Set the stream by sharing ownership. */
	void share(std::shared_ptr<std::ostream> out) noexcept {
		std::lock_guard<std::mutex> guard(m_mtxStream);
		m_pstream = out;
		m_pout = out.get();
	}

	/** Sets a stream without holding the ownership. */
	void writeOn(std::ostream* out) noexcept {
		std::lock_guard<std::mutex> guard(m_mtxStream);
		m_pout = out;
	}

    virtual void onMessage( const Falcon::LogSystem::Message& msg ) override;

private:

    // Initialise the output to sink
    NullBuffer m_nullBuffer;
    std::ostream m_dummy{&m_nullBuffer};
    std::ostream *m_pout{&m_dummy};

    std::mutex m_mtxStream;
    std::unique_ptr<std::ostream> m_uptrStream;
    std::shared_ptr<std::ostream> m_pstream;
};

}

#endif /* _FALCON_LOGSTREAM_H_ */

/* end of logstream.h */

