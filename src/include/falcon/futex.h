/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: futex.h

  Lightweight non-blocking mutex
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Fri, 12 Apr 2019 13:01:55 +0100
  Touch : Fri, 12 Apr 2019 13:01:55 +0100

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_FUTEX_H_
#define _FALCON_FUTEX_H_

#include <atomic>

namespace Falcon {
/**
 * Lightweight non-blocking mutex
 */
class Futex {
   std::atomic<bool> m_status{false};

public:
   Futex() {}
   Futex(const Futex& )= delete;
   Futex(Futex&& )= delete;
   ~Futex() {}

   void lock() noexcept {
      bool locker = false;
      while(!m_status.compare_exchange_weak(locker, true,
            std::memory_order_acq_rel, std::memory_order_relaxed));
   }

   void unlock() noexcept {
      m_status.store(false, std::memory_order_release);
   }

   bool isLocked() noexcept {
      return m_status.load(std::memory_order_acquire);
   }
};

}

#endif /* _FALCON_FUTEX_H_ */

/* end of futex.h */
