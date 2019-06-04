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
#include <falcon/logger.h>
#include <type_traits>

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
   bool doStart = false;
   if (m_isStarted.compare_exchange_strong(doStart, true)) {
      // became true
      m_mainThread = std::thread(&GarbageCollector::run, this);
      return true;
   }

   return false;
}

bool GarbageCollector::stop() {
   bool doStart = true;
   if (m_isStarted.compare_exchange_strong(doStart, false)) {
      m_messageQueue.emplace(StopMessage());
      m_mainThread.join();
      return true;
   }
   return false;
}

std::future<size_t> GarbageCollector::forceCollection() noexcept
{
   CollectMessage cmsg;
   auto fut = cmsg.collected->get_future();
   m_messageQueue.push(cmsg);
   return fut;
}


// VISITOR HELPERS; hopefully, something like this will found its way in the standard
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void GarbageCollector::run() {
   bool running = true;

   while(running) {
      Message msg = m_messageQueue.get();

      std::visit( overloaded{
         [&](StopMessage&) { LOG_INFO << "Received StopMessage"; running = false;},
         [&](CollectMessage& msg) { LOG_INFO << "Received CollectMessage"; msg.collected->set_value(0);},
      }, msg);
   }
}

}


/* end of gc.cpp */
