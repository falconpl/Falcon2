/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logsystem.cpp

  Falcon logging system
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 23 Feb 2019 12:55:51 +0000
  Touch : Sat, 02 Mar 2019 01:33:45 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/logsystem.h>
#include <algorithm>

namespace Falcon {

LogSystem::LogSystem(bool startNow):
		m_logThread(0),
		m_level(LEVEL::TRACE),
		m_unpooled(0),
		m_destroyed(0),
		m_maxMsgQueueSize(0),
		m_msgReceived(0)
{
	// prepare the pool
	for (int i = 0; i < MESSAGE_POOL_THRESHOLD; ++i) {
		m_pool.push_back(new Message);
	}

	if(startNow) {
		start();
	}
}


LogSystem::~LogSystem()
{
	stop();

	// delete the pending messages
	for(auto* message: m_messages){
		delete message;
	}

	// and delete the pool
	for(auto* message: m_pool){
		delete message;
	}
}


void LogSystem::start() {
	std::lock_guard<std::mutex> guard(m_mtxThread);
	if(m_logThread == 0) {
		m_logThread = new std::thread(&LogSystem::loggingThread, this);
	}
}


void LogSystem::stop() noexcept {
	// send a killer message
	m_mtxMessage.lock();
	m_isTerminated = true;
	m_mtxMessage.unlock();
	m_cvLogs.notify_all();

	// now
	std::thread* the_thread = 0;
	std::lock_guard<std::mutex> guard(m_mtxThread);
	if(m_logThread == 0) {
		return;
	}

	// we join in a lock, but this stop() should hardly be called more than once per program.
	m_logThread->join();
	delete m_logThread;
	m_logThread = 0;
}


void LogSystem::log( const std::string& file, int line, LEVEL level, const std::string& cat, const std::string& message )
{
	// Ignore anything above our current log level.
	if (level > m_level) {
		return;
	}

	Message* msg = new(allocateMsg()) Message(file, line, level, cat, message);
	log(msg);
}


void LogSystem::log( LogSystem::Message* msg ) noexcept
{
	std::lock_guard<std::mutex> guard(m_mtxMessage);
	m_messages.push_back(msg);
	if(m_messages.size() > m_maxMsgQueueSize) {
		m_maxMsgQueueSize = m_messages.size();
	}
	m_cvLogs.notify_all();
}

/**
 * Utility providing a 4 letter level description of a log level.
 *
 * @param lvl The level to be turned into a string.
 * @return The description, or "????" if unknonwn.
 *
 * The possible return values are:
 * - "CRIT"
 * - "ERR "
 * - "WARN"
 * - "INFO"
 * - "DBG "
 * - "TRC "
 * - "????"
 */
const char* LogSystem::levelToString( LogSystem::LEVEL lvl )
{
	switch(lvl) {
	case LEVEL::CRITICAL: return "CRIT";
	case LEVEL::ERROR: return "ERR ";
	case LEVEL::WARN: return "WARN";
	case LEVEL::INFO: return "INFO";
	case LEVEL::DEBUG: return "DBG ";
	case LEVEL::TRACE: return "TRC ";
	}

	return "????";
}


void LogSystem::addListener(std::shared_ptr<LogSystem::Listener> l)
{
	std::lock_guard<std::mutex> guard(m_mtxListeners);
	m_pendingListeners.push_back(l);
	// We are not interested in waking up the other thread,
	// it will carry out this operation when necessary.
}



void LogSystem::loggingThread() noexcept
{
	while(true) {
		std::unique_lock<std::mutex> msgl(m_mtxMessage);
		m_cvLogs.wait(msgl, [=](){
			return (!m_messages.empty()) || m_isTerminated;
			}
		);

		// check for termination
		if (m_isTerminated) {
			return;
		}

		Message* msg = m_messages.front();
		m_messages.pop_front();
		msgl.unlock();

		// Do we need to add new listeners?
		processNewListeners();

		// Now send the message
		sendMessageToListeners(msg);

		// ... or remove the dead ones?
		cleanupTerminatedListeners();

		// give back to the pool
		disposeMsg(msg);
	}
}


void LogSystem::processNewListeners() noexcept {
	{
		std::lock_guard<std::mutex> guard(m_mtxListeners);
		std::copy(m_pendingListeners.begin(), m_pendingListeners.end(),
				std::back_inserter(m_activeListeners));
		m_pendingListeners.clear();
	}

}

void LogSystem::sendMessageToListeners(Message* msg) noexcept
{
	m_msgReceived++;
	for(auto listener: m_activeListeners) {
		if(listener->isEnabled()
				&& listener->level() >= msg->m_level
				&& listener->checkCategory(msg->m_category))
		{
			listener->onMessage(*msg);
		}
	}
}

bool LogSystem::Listener::checkCategory(const std::string& cat) const noexcept
{
	  if(cat == "") return true;
	  std::lock_guard<std::mutex> guard(m_mtxCategory);
	  if(m_category == "") {
		  return true;
	  }
	  std::cout << "Category filtering: " << cat <<" vs. "<< m_category << ": " << std::regex_match(cat, m_catRegex)<<  '\n';
	  return std::regex_match(cat, m_catRegex);
  }



LogSystem::Message* LogSystem::allocateMsg()
{
	{
		std::lock_guard<std::mutex> guard(m_mtxPool);
		if(!m_pool.empty()) {
			Message* msg = m_pool.back();
			m_pool.pop_back();
			return msg;
		}
	}
	m_unpooled++;
	return new Message;
}


void LogSystem::disposeMsg(Message* msg) noexcept
{
	{
		std::lock_guard<std::mutex> guard(m_mtxPool);
		if(m_pool.size() < MESSAGE_POOL_THRESHOLD) {
			m_pool.push_back(msg);
			return;
		}
	}
	m_destroyed++;
	delete msg;
}


void LogSystem::cleanupTerminatedListeners()
{
	m_activeListeners.erase(
			std::remove_if(m_activeListeners.begin(), m_activeListeners.end(),
					[](auto& listener) { return listener->isDetached(); }),
			m_activeListeners.end());

}


void LogSystem::getDiags(LogSystem::Diags& diags) noexcept
{
	diags.m_msgsCreated = m_unpooled;
	diags.m_msgsDiscarded = m_destroyed;
	diags.m_activeListeners = m_activeListeners.size();
	diags.m_enabledListeners = std::count_if(m_activeListeners.begin(), m_activeListeners.end(),
				[](const auto& l){return l->isEnabled();});
	diags.m_msgReceived = m_msgReceived;

	{
		std::lock_guard<std::mutex> guard(m_mtxMessage);
		diags.m_maxMsgQueueSize = m_maxMsgQueueSize;
	}
	{
		std::lock_guard<std::mutex> guard(m_mtxPool);
		diags.m_poolSize = m_pool.size();
	}
	{
		std::lock_guard<std::mutex> guard(m_mtxListeners);
		diags.m_pendingListeners = m_pendingListeners.size();
	}
}

}

/* end of logsystem.cpp */
