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
#include <thread>
#include <iostream>



namespace falcon {
/**
 * Lightweight non-blocking mutex.
 *
 * The Spincount template parameter controls the spincount behavior.
 *
 * With zero, it's off, and the futex behaves as a pure idle loop/futex.
 *
 * With one, the thread yields whenever it detects contention on the
 * futex.
 *
 * With more than one, the a contended lock is idle-spun down to zero,
 * and if still unsuccessful, the thread is yielded.
 */
template<unsigned int spinCount=0>
class Futex {
   std::atomic<bool> m_owned{false};

public:
   Futex() {}
   Futex(const Futex& )= delete;
   Futex(Futex&& )= delete;
   ~Futex() {}

   void lock() noexcept {
      bool isOwned = false;
      unsigned int sc = spinCount;
      while(!m_owned.compare_exchange_weak(
            isOwned, true,
            std::memory_order_acq_rel,
            std::memory_order_relaxed) || isOwned ) {
         isOwned = false;

         if(spinCount && --sc == 0) {
            std::this_thread::yield();
         }
      }
   }

   void unlock() noexcept {
      m_owned.store(false, std::memory_order_release);
   }

   bool isLocked() noexcept {
      return m_owned.load(std::memory_order_acquire);
   }
};


class RFutex {
   static std::atomic<unsigned int> s_count;
   static thread_local unsigned int s_thread_id;
   std::atomic<unsigned int> m_owner{0};
   unsigned int m_count{0};

public:
   RFutex() {}
   RFutex(const RFutex& )= delete;
   RFutex(RFutex&& )= delete;
   ~RFutex() {}

   void lock() noexcept {
      if (s_thread_id == 0) {
         s_thread_id = ++s_count;
      }

      unsigned int free = 0;
      unsigned int thread_id = s_thread_id;

      while(!m_owner.compare_exchange_strong(
            free, thread_id,
            std::memory_order_seq_cst,
            std::memory_order_seq_cst) || free != thread_id) {
         free = 0;
      }
      ++m_count;
   }

   void unlock() noexcept {
      if(m_count > 0 && --m_count == 0) {
         m_owner.store(0, std::memory_order_seq_cst);
      }
   }

   bool isLocked() noexcept {
      return m_owner.load(std::memory_order_seq_cst) != 0;
   }

   bool isOwner() noexcept {
      return m_owner.load(std::memory_order_seq_cst) == s_thread_id;
   }
};

}

#endif /* _FALCON_FUTEX_H_ */

/* end of futex.h */
