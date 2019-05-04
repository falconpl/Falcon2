/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: gc.cpp

  Generic Garbage Collector
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 13 Apr 2019 09:17:23 +0100
  Touch : Sat, 13 Apr 2019 09:17:23 +0100

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/engine/gc.h>

namespace Falcon {
/*
GarbageCollector::HandlerID GarbageCollector::registerHandler(std::unique_ptr<Handler> handler) noexcept
{
   std::lock_guard guard(m_mtxHandlers);
   auto size =  static_cast<GarbageCollector::HandlerID>(m_handlers.size());
   m_handlers.push_back(std::move(handler));
   return size;
}
*/

GarbageCollector::GarbageCollector(bool s)
{
   if (s) {
      start();
   }
}


GarbageCollector::~GarbageCollector() {
   stop();
}


bool GarbageCollector::start() {
   bool doStart = true;
   if (!m_isStarted.compare_exchange_strong(doStart, false)) {
      return false;
   }

   m_mainThread = std::thread(&GarbageCollector::run, this);

   return true;
}

bool GarbageCollector::stop() {
   bool doStart = false;
   if (!m_isStarted.compare_exchange_strong(doStart, true)) {
      return false;
   }

   // TODO send the message.
   m_mainThread.join();
   return true;
}


void GarbageCollector::run() {

   while(true) {

   }
}

}


/* end of gc.cpp */
