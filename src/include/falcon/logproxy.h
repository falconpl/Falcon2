/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logproxy.h

  Log listener using another listener's message facility
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 02 Mar 2019 14:04:17 +0000
  Touch : Sat, 02 Mar 2019 14:04:17 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_LOGPROXY_H_
#define _FALCON_LOGPROXY_H_

#include <falcon/logsystem.h>

namespace Falcon {

/**
 *   Log listener using another listener's message facility.
 *
 *   This Listener "copies" another listener, sending all the messages
 *   it filters to the other one. It is used to implement multiple filter
 *   rules while still using an existing, different handler, which is stored
 *   on a listener that doesn't necessary receives log messages from the
 *   underlying system -- or has different filtering rules.
 *
 */
class LogProxyListener: public LogSystem::Listener
{
public:
	/** Creates a proxy with a shared pointer to another listener */
	LogProxyListener(LogSystem::PListener other):
		m_other(other)
	{}

    virtual void onMessage( const LogSystem::Message& msg ) override {
    	m_other->onMessage(msg);
    }

private:
    LogSystem::PListener m_other;
};

}

#endif /* _FALCON_LOGPROXY_H_ */

/* end of logproxy.h */
