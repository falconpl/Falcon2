/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: log.cpp

  Falcon logging system
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 23 Feb 2019 12:55:51 +0000
  Touch : Sat, 23 Feb 2019 17:36:46 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <log.h>

#include <algorithm>

namespace Falcon {

LogSystem::LogSystem(bool startNow):
		m_logThread(0),
		m_level(LEVEL::TRACE)
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
	for(auto iter: m_messages){
		delete *iter;
	}

	// and delete the pool
	for(auto iter: m_pool){
		delete *iter;
	}
}


void LogSystem::start() {
	std::lock_guard<std::mutex> guard(m_mtxThread);
	if(m_logThread == 0) {
		m_logThread = new std::thread(LogSystem::loggingThread, this);
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
		msgl.lock();
		m_cvLogs.wait(msgl, [](){
			return ! m_messages.empty() || m_isTerminated;
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

		// ... or remove the dead ones?
		cleanupTerminatedListeners();

		// Now send the message
		sendMessageToListeners(msg);

		// give back to the pool
		disposeMsg(msg);
	}
}


void LogSystem::processNewListeners() noexcept {
	{
		std::lock_guard<std::mutex> guard(m_mtxListeners);
		std::copy(m_activeListeners.end(), m_pendingListeners.begin(), m_pendingListeners.end());
		m_pendingListeners.clear();
	}

}

void LogSystem::sendMessageToListeners(Message* msg) noexcept {
	// Perform our loop.
	for(auto listener: m_activeListeners) {
		if(listener->isEnabled()
				&& listener->level() <= msg->m_level
				&& checkCategory(listener->m_category, msg->m_category))
		{
			listener->onMessage(*msg);
		}
	}
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
	delete msg;
}

void LogSystem::cleanupTerminatedListeners()
{
	std::remove_if(m_activeListeners.begin(), m_activeListeners.end(),
	      [](auto& listener) { return listener->m_detached; });
}

bool LogSystem::checkCategory(std::shared_ptr<std::string>listenerCat, const std::string& msgCat)
{
	return msgCat.empty() || *listenerCat == msgCat;
}

/* end of log.cpp */
