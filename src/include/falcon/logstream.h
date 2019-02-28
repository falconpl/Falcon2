/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logstream.h

  Log Listener writing data to a stream
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 24 Feb 2019 14:44:02 +0000
  Touch : Thu, 28 Feb 2019 20:59:11 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_LOGSTREAM_H_
#define _FALCON_LOGSTREAM_H_

#include "logsystem.h"

namespace Falcon {

class FALCON_API_ LogStreamListener: public LogSystem::Listener
{
public:
	LogStreamListener(std::shared_ptr<std::ostream> pos):
		m_postream(pos)
	{}

protected:
    virtual void onMessage( const Falcon::LogSystem::Message& msg ) override;

private:
	std::shared_ptr<std::ostream> m_postream;
};

}

#endif /* _FALCON_LOGSTREAM_H_ */

/* end of logstream.h */

