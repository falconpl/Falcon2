/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: ihandler.cpp

  Base Item Handler
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 16 Mar 2019 15:09:25 +0000
  Touch : Sat, 16 Mar 2019 15:09:25 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/engine/ihandler.h>
#include <falcon/engine/vmcontext.h>
#include <falcon/logger.h>

namespace Falcon {

void IHandler::addMessage(const std::string& msg, IHandler::MessageHandler mh) noexcept
{
	LOG_CATEGORY("M+ENG");
	LOG_TRC << this << "->" << "addMessage(" << msg << ", " << mh << ")";

	if(!m_messageMap.insert(std::make_pair(msg, mh)).second){
		LOG_DBG << "addMessage: substituting " << msg << " with " << mh;
	}

}


void IHandler::delMessage(const std::string& msg) noexcept
{
	LOG_CATEGORY("M+ENG");
	LOG_TRC << this << "->" << "delMessage(" << msg << ")";

	m_messageMap.erase(msg);
}

void IHandler::propertyDispatcher(const std::string& msg, VMContext& ctx)
{
	LOG_CATEGORY("M+ENG");
	LOG_TRC << this << "->" << "propertyDispatcher(" << msg << ", " << ctx << ", " << ") ";
	Item prop;

	if(getProperty(ctx.self(), msg, prop)) {

	}
}

bool IHandler::sendMessage(const std::string& msg, VMContext& ctx) noexcept
{
	LOG_CATEGORY("M+ENG");

	MessageMap::const_iterator pos = m_messageMap.find(msg);
	if (pos != m_messageMap.end()) {
		LOG_TRC << "sendMessage(" << msg << ", " << ctx << ") to " << pos->second;
		pos->second(msg, ctx);
		return true;
	}

	LOG_TRC << this << "->" << "sendMessage(" << msg << ", " << ctx ") to default ";
	return m_defaultHandler(msg, ctx);
}

}


/* end of ihandler.cpp */
