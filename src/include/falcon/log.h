/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: log.h

  Falcon logging subsystem
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 23 Feb 2019 10:30:32 +0000
  Touch : Sat, 23 Feb 2019 17:36:46 +0000

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

	   Message(const std::string& file, int line, LEVEL level, const std::string& cat, const String& message ):
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
	  	  m_category(""),
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
    	  // TODO: compile the regex
    	  // The unique pointer is atomically set.
    	  m_category = std::make_shared<std::string>(regex_cat);
      }

      /**
       * Returns the current category filter
       */
      const std::string& category() const noexcept {
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
      LEVEL level() const noexcept { return m_level;}

   protected:
      virtual void onMessage( const Message& msg ) = 0;

   private:
      std::shared_ptr<std::string> m_category;
      std::atomic<LEVEL> m_level;
      std::atomic<bool> m_enabled;
      std::atomic<bool> m_detached;

      friend class Log;
   };

   /**
    * Send a log message.
    */
   void log( const std::string& file, int line, LEVEL level, const std::string& cat, const std::string& message ) noexcept;
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

private:

   void loggingThread() noexcept;
   Message* allocateMsg();
   void disposeMsg(Message*) noexcept;
   void cleanupTerminatedListeners();
   void sendMessageToListeners(Message* msg) noexcept;
   void processNewListeners();

   static bool checkCategory(std::shared_ptr<std::string>listenerCat, const std::string& msgCat);

   /** Maximum number of pre-allocated message */
   enum {
	   MESSAGE_POOL_THRESHOLD = 64
   };

   /* Current log level */
   std::atomic<LEVEL> m_level;
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
