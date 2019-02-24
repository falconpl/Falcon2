/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: log.h

  Falcon logging subsystem
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 23 Feb 2019 10:30:32 +0000
  Touch : Sun, 24 Feb 2019 12:42:56 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_LOG_H_
#define _FALCON_LOG_H_

#include <falcon/setup.h>
#include <falcon/engine/singleton.h>

#include <atomic>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <regex>
#include <string>
#include <thread>

namespace Falcon {

class String;

/**
 * Engine-level pluggable asynchronous logging facility.
 *
 * The log subsystem has capabilities for runtime AND compile-time
 * log level filtering, so to optimise the level of logging depending
 * on the status of the development.
 *
 * The class itself is used also by the language-level log module.
 */
class FALCON_API_ LogSystem
{
public:
   LogSystem(bool startNow=true);
   ~LogSystem();

   using LEVEL = enum {
	   CRITICAL 	= 0,
	   ERROR 		= 1,
	   WARN 		= 2,
	   INFO 		= 3,
	   DEBUG 		= 4,
	   TRACE 		= 5
   };

   /** A log message has information about the message source and level.
    *
    * It is stored in a pool of messages that is then reused.
    */
   struct Message
   {
	   Message():
		   m_line(0),
		   m_level(CRITICAL)
	   {}

	   Message(const std::string& file, int line, LEVEL level, const std::string& cat, const std::string& message ):
		   m_file(file),
		   m_line(line),
		   m_level(level),
		   m_category(cat),
		   m_message(message)
	   {}
	   Message(const Message& ) = default;
	   Message(Message&& ) = default;
       ~Message() = default;

	   std::string m_file;
	   int m_line;
	   LEVEL m_level;
	   std::string m_category;
	   std::string m_message;
   };

   /**
    * Listener base class.
    *
    * Log data is streamed through listeners, that might be interested
    * in storing the information received by a log line.
    *
    */
   class Listener
   {
   public:
      Listener() noexcept :
		  m_level(LEVEL::TRACE),
		  m_enabled(true),
		  m_detached(false)
	  {}
      Listener(const Listener& ) = delete;
      Listener(Listener&& ) = delete;
      virtual ~Listener() = default;

      /**
       * Filter for categories. Leave empty to accept all categories.
       *
       * Will throw if the regular expression is not valid.
       */
      void category( const std::string& regex_cat )
      {
    	  std::lock_guard<std::mutex> guard(m_mtxCategory);
    	  // may throw in case of error
    	  m_catRegex = std::regex(regex_cat);
    	  m_category = regex_cat;
      }

      /**
       * Returns the current category filter
       */
      const std::string& category() const noexcept {
    	  std::lock_guard<std::mutex> guard(m_mtxCategory);
    	  return m_category;
      }


      virtual void enable(bool mode) {
    	  m_enabled = mode;
      }

      bool isEnabled() const noexcept {return m_enabled;}

      virtual void detach() {m_detached = true;}

      /**
       * Sets a minimal log level for this listener.
       *
       * This listener's onMessage will be only called back when the
       * incoming message level is LESS or EQUAL to the given level.
       *
       * @note As this method is not synchronised, time might pass before
       * the filter level change is actually enforced.
       */
      void level(LEVEL l) noexcept {m_level = l;}

      /** Get the current minimum log level */
      LEVEL level() const noexcept {return m_level;}

      bool isDetached() const noexcept {return m_detached;}

      bool checkCategory(const std::string& cat) const noexcept {
    	  if(cat == "") return true;
    	  std::lock_guard<std::mutex> guard(m_mtxCategory);
    	  if(m_category == "") {
    		  return true;
    	  }
    	  return std::regex_match(cat.begin(), cat.end(), m_catRegex);
      }

   protected:
      virtual void onMessage( const Message& msg ) = 0;

   private:
      mutable std::mutex m_mtxCategory;
      std::string m_category;
      std::regex m_catRegex;
      std::atomic<LEVEL> m_level;
      std::atomic<bool> m_enabled;
      std::atomic<bool> m_detached;
      friend class LogSystem;
   };

   /**
    * Send a log message.
    */
   void log( const std::string& file, int line, LEVEL level, const std::string& cat, const std::string& message );
   void log( Message* msg ) noexcept;

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
   static const char* levelToString( LEVEL lvl );

   /** Adds a listener for incoming logs.
    * @param l The listener to be added
    */
   void addListener(std::shared_ptr<Listener> l);

   /**
    * Sets a minimal log level for this listener.
    *
    * This listener's onMessage will be only called back when the
    * incoming message level is LESS or EQUAL to the given level.
    *
    * @note As this method is not synchronised, time might pass before
    * the filter level change is actually enforced.
    */
   void level(LEVEL l) noexcept {m_level = l;}

   /** Get the current minimum log level */
   LEVEL level() const noexcept { return m_level;}

   /** Starts the service */
   void start();

   /** Stops the service */
   void stop() noexcept;


   /** Maximum number of pre-allocated message */
   enum {
	   MESSAGE_POOL_THRESHOLD = 64
   };

   /** Stucture used for reporting the internal status of the logger.
    *
    */
   struct Diags {
	   size_t m_poolSize;
	   size_t m_maxMsgQueueSize;
	   size_t m_msgReceived;

	   size_t m_msgsCreated;
	   size_t m_msgsDiscarded;
	   size_t m_pendingListeners;
	   size_t m_activeListeners;
	   size_t m_enabledListeners;
   };

   /** A diagnostics function to check for the health of the logger.
    *
    * TODO: expand with performance checks.
    */
   void getDiags(Diags& diags) noexcept;

private:

   void loggingThread() noexcept;
   Message* allocateMsg();
   void disposeMsg(Message*) noexcept;
   void cleanupTerminatedListeners();
   void sendMessageToListeners(Message* msg) noexcept;
   void processNewListeners() noexcept;

   /* Current log level */
   std::atomic<LEVEL> m_level;
   std::atomic<size_t> m_unpooled;
   std::atomic<size_t> m_destroyed;
   size_t m_maxMsgQueueSize;
   size_t m_msgReceived;


   using MessageQueue = std::deque<Message*>;
   MessageQueue m_messages;
   bool m_isTerminated;
   std::condition_variable m_cvLogs;
   mutable std::mutex m_mtxMessage;

   mutable std::mutex m_mtxPool;
   MessageQueue m_pool;


   using ListenerList = std::deque<std::shared_ptr<Listener>>;
   ListenerList m_pendingListeners;
   ListenerList m_activeListeners;

   std::thread* m_logThread;
   mutable std::mutex m_mtxThread;

   // Mutex controlling adding listeners.
   mutable std::mutex m_mtxListeners;
};

}

#endif /* _FALCON_LOG_H_ */

/* end of log.h */
