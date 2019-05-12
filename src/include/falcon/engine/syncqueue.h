/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: syncqueue.h

  Synchronised Queue
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 04 May 2019 18:56:10 +0100
  Touch : Sat, 04 May 2019 18:56:10 +0100

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_SYNCQUEUE_H_
#define _FALCON_SYNCQUEUE_H_

#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>

namespace Falcon {

template<typename _vt>
class sync_queue
{
public:
   using value_type = _vt;
   using reference_type = value_type&;

   sync_queue()=default;
   ~sync_queue()=default;
   sync_queue(const sync_queue&)=delete;
/*
   sync_queue(sync_queue&& other):
   {
      std::lock_guard guard(other.m_mtx);
      m_queue.insert(v2.end(), std::make_move_iterator(other.m_queue.begin()),
                     std::make_move_iterator(other.m_queue.end()));
      other.m_queue.clear();
   }
   */

   template<class... Args>
   reference_type emplace(Args&&... args)
   {
      std::lock_guard guard(m_mtx);
      m_queue.emplace_back(std::forward<Args...>(args...));
      reference_type back = m_queue.back();
      m_cvNotEmpty.notify_all();
      return back;
   }

   void push(const value_type& v) noexcept
   {
      {
         std::lock_guard guard(m_mtx);
         m_queue.push_front(v);
         reference_type back = m_queue.front();
      }
      m_cvNotEmpty.notify_all();
   }


   value_type get() noexcept
   {
      std::unique_lock guard(m_mtx);
      m_cvNotEmpty.wait(guard, [&](){return !m_queue.empty();});
      value_type msg = m_queue.back();
      m_queue.pop_back();
      return msg;
   }

   /** Wait for the given time span, or until a message is ready. */
   template<typename _rep, typename _period>
   std::optional<value_type> get(std::chrono::duration<_rep, _period> dur) {
      std::unique_lock guard(m_mtx);
      if(!m_cvNotEmpty.wait_for(guard, dur, [&](){return !m_queue.empty();})) {
         return std::nullopt;
      }

      value_type msg = m_queue.back();
      m_queue.pop_back();
      return msg;
   }

private:
   std::mutex m_mtx;
   std::condition_variable m_cvNotEmpty;
   std::deque<value_type> m_queue;

};

}

#endif /* _FALCON_SYNCQUEUE_H_ */

/* end of syncqueue.h */

