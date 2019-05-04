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

GarbageCollector::GarbageCollector
{
}

GarbageCollector::~GarbageCollector() {
   stop();
}


}


/* end of gc.cpp */
