/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: pagedstack.h

  Growable Stack, ready for use in the engine
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Fri, 05 Apr 2019 19:54:01 +0100
  Touch : Fri, 05 Apr 2019 19:54:01 +0100

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
 ******************************************************************************/
#include <vector>
#include <list>
#include <cassert>
#include <memory>
#include <falcon/engine/distribute.h>
#include <mutex>

#ifndef _FALCON_PAGEDSTACK_H_
#define _FALCON_PAGEDSTACK_H_

namespace Falcon {
/**
 * Stack-like structure indefinitely growable.
 *
 */

namespace{
class dummy_mutex {
public:
	void lock() const volatile noexcept {};
	void unlock() const volatile noexcept {};
};
}

template<typename _T,
	template<typename> typename _Allocator=std::allocator, typename _Mutex=dummy_mutex>
class PagedStack
{
public:
   enum {
      DEFAULT_PAGE_SIZE=1024,
      DEFAULT_BASE_SIZE=4
   };

   using value_type = _T;
   using reference_type = _T&;

   using allocator_type = _Allocator<_T>;
private:

   using page_type = std::vector<_T, allocator_type>;
   using page_allocator_type = typename allocator_type::template rebind<page_type>::other;
   using base_type = std::list<page_type, page_allocator_type>;
   size_t m_pageSize{DEFAULT_PAGE_SIZE};
   size_t m_allocSize{DEFAULT_BASE_SIZE};
   base_type m_base;
   typename base_type::iterator m_curBase;
   allocator_type m_dataAllocator;
   mutable _Mutex m_mutex;

   void growBase() {
      size_t count = m_allocSize;
      while(count) {
         m_base.emplace_back(m_dataAllocator);
         m_base.back().reserve(m_pageSize);
         --count;
      }
   }

   void advance() {
      if (m_curBase->size() == m_pageSize) {
         ++m_curBase;
         if(m_curBase == m_base.end()) {
            m_curBase--;
            growBase();
            m_curBase++;
         }
      }
   }

   template<typename... _Args>
   void internal_pop(_T& value, _Args&&... __args) {
      value = m_curBase->back();
      internal_pop_one();
      internal_pop(std::forward<_Args>(__args)...);
   }

   void internal_pop_one() {
      m_curBase->pop_back();
      if(m_curBase->empty() && m_curBase != m_base.begin()) {
         --m_curBase;
      }
   }

   void internal_pop() {}

   template<typename _IBase, typename _IData>
   void internal_discard(_IBase iBase, _IData iData)
   {
      std::lock_guard<_Mutex> guard(m_mutex);
      while(m_curBase != iBase) {
         m_curBase->clear();
         --m_curBase;
      }
      if(iData == m_curBase->begin())
      {
         if (m_curBase != m_base.begin())
         {
            m_curBase--;
         }
         else {
            m_curBase->clear();
         }
      }
      else {
         m_curBase->resize(iData - m_curBase->begin());
      }
   }

   void internal_push(const _T& data) {
      advance();
      m_curBase->push_back(data);
   }

   template<typename... _Args>
   void internal_push(const _T& data, _Args&&... __args) {
      advance();
      m_curBase->push_back(data);
      internal_push(std::forward<_Args>(__args)...);
   }

   template<typename _TT, typename _IBase, typename _IData>
   class reverse_iterator_base {
   public:
      _TT& operator*() noexcept { return *m_iData; }
      const _TT& operator*() const noexcept { return *m_iData; }
      _TT* operator->() const noexcept { return &(*m_iData); }

      bool operator ==(const reverse_iterator_base& other) const noexcept {
         return m_iBase == other.m_iBase && m_iData == other.m_iData;
      }

      bool operator !=(const reverse_iterator_base& other) const noexcept {
         return !(*this == other);
      }

      const reverse_iterator_base& operator+=(size_t size) noexcept {
         for(size_t i=0; i < size; ++i) {++ *this;}
         return *this;
      }

      const reverse_iterator_base& operator-=(size_t size) noexcept {
         for(size_t i=0; i < size; ++i) {-- *this;}
         return *this;
      }

      reverse_iterator_base operator++(int) noexcept {
         reverse_iterator_base current = *this;
         ++(*this);
         return current;
      }

      reverse_iterator_base operator--(int) noexcept {
         reverse_iterator_base current = *this;
         --(*this);
         return current;
      }


      const reverse_iterator_base& operator--() noexcept {
         if(m_iData == m_iBase->begin()) {
            --m_iBase;
            m_iData = m_iBase->end();
         }
         --m_iData;
         return *this;
      }

      const reverse_iterator_base& operator++() noexcept {
         ++m_iData;
         if(m_iData == m_iBase->end() && m_iBase != m_iBaseEnd) {
            ++m_iBase;
            m_iData = m_iBase->begin();
         }
         return *this;
      }

   private:
      reverse_iterator_base(const _IBase& iBase,
            const _IData& iData,
            const _IBase& end) noexcept:
            m_iBase(iBase),
            m_iData(iData),
            m_iBaseEnd(end)
      {}

      _IBase m_iBase;
      _IData m_iData;
      _IBase m_iBaseEnd;

      friend class PagedStack;
   };

   template<typename _TT, typename _IBase, typename _IData>
   class iterator_base {
   public:

      _TT& operator*() noexcept { return *m_iData; }
      const _TT& operator*() const noexcept { return *m_iData; }
      _TT* operator->() const noexcept { return &(*m_iData); }

      bool operator ==(const iterator_base& other) const noexcept {
         return m_iBase == other.m_iBase && m_iData == other.m_iData;
      }

      bool operator !=(const iterator_base& other) const noexcept {
         return !(*this == other);
      }

      const iterator_base& operator+=(size_t size) noexcept {
         for(size_t i=0; i < size; ++i) {++ *this;}
         return *this;
      }

      const iterator_base& operator-=(size_t size) noexcept {
         for(size_t i=0; i < size; ++i) {-- *this;}
         return *this;
      }

      iterator_base operator++(int) noexcept {
         iterator_base current = *this;
         ++(*this);
         return current;
      }

      iterator_base operator--(int) noexcept {
         iterator_base current = *this;
         --(*this);
         return current;
      }

      const iterator_base& operator++() noexcept {
         if(m_iData == m_iBase->begin() && m_iBase != m_iBaseBegin) {
            --m_iBase;
            m_iData = m_iBase->end();
         }
         --m_iData;
         return *this;
      }

      const iterator_base& operator--() noexcept {
         ++m_iData;
         if(m_iData == m_iBase->end()) {
            ++m_iBase;
            m_iData = m_iBase->begin();
         }
         return *this;
      }
   private:

      iterator_base(const _IBase& iBase,
            const _IData& iData,
            const _IBase& end) noexcept:
            m_iBase(iBase),
            m_iData(iData),
            m_iBaseBegin(end)
      {}

      _IBase m_iBase;
      _IData m_iData;
      _IBase m_iBaseBegin;
      friend class PagedStack;
   };

   template<typename _TT, typename _IBase, typename _IData>
     class sync_iterator_base: public iterator_base< _TT, _IBase, _IData> {
     public:
	   sync_iterator_base(const sync_iterator_base& other):
		   iterator_base< _TT, _IBase, _IData>(other)
	   {}
     private:
	   sync_iterator_base(PagedStack const* owner,
  			      const _IBase& iBase,
  	              const _IData& iData,
  	              const _IBase& end) noexcept:
  				  iterator_base< _TT, _IBase, _IData>(iBase, iData, end)
  	   {}


  	   friend class PagedStack;
     };
public:

   using iterator = iterator_base<_T, typename base_type::iterator, typename page_type::iterator>;
   using const_iterator = iterator_base<const _T, typename base_type::const_iterator, typename page_type::const_iterator>;
   using reverse_iterator = reverse_iterator_base<_T, typename base_type::iterator, typename page_type::iterator>;
   using const_reverse_iterator = reverse_iterator_base<const _T, typename base_type::const_iterator, typename page_type::const_iterator>;
   using sync_iterator = sync_iterator_base<_T, typename base_type::iterator, typename page_type::iterator>;
   using const_sync_iterator = sync_iterator_base<const _T, typename base_type::const_iterator, typename page_type::const_iterator>;


   PagedStack(size_t pageSize = DEFAULT_PAGE_SIZE, size_t prealloc = DEFAULT_BASE_SIZE,
         const allocator_type& dataAllocator = allocator_type() ):
            m_pageSize(pageSize),
            m_allocSize(prealloc),
            m_base(page_allocator_type(dataAllocator)),
            m_dataAllocator(dataAllocator)
   {
      growBase();
      m_curBase = m_base.begin();
   }

   _T& top() noexcept { std::lock_guard<_Mutex> guard(m_mutex); return m_curBase->back(); }
   const _T& top() const noexcept { std::lock_guard<_Mutex> guard(m_mutex); return m_curBase->back(); }

   void push(const _T& data) {
      std::lock_guard<_Mutex> guard(m_mutex);
      internal_push(data);
   }

   /**
    * Push multiple values at the end.
    *
    * The last parameter will be on top of the stack.
    */
   template<typename... _Args>
   void push(const _T& data, _Args&&... __args) {
      std::lock_guard<_Mutex> guard(m_mutex);
      internal_push(data, std::forward<_Args>(__args)...);
   }

   /**
    * Constructs an entry and places it on top of the stack.
    */
   template<typename... _Args>
   void push_emplace(_Args&&... __args)	{
      std::lock_guard<_Mutex> guard(m_mutex);
      advance();
      // vector::emplace writes at the previous iterator.
      m_curBase->emplace_back(std::forward<_Args>(__args)...);
   }

   /**
    * Removes the last element from the stack.
    *
    * Popping on an empty stack is an undefined behaviour.
    */
   void pop() {
      assert(!empty());
      std::lock_guard<_Mutex> guard(m_mutex);
      internal_pop_one();
   }

   /**
    * Pop multiple values from the top of the stack.
    *
    * The first parameter will receive the element on top of the stack.
    *
    * Popping from an empty stack has undefined behaviour.
    */
   template<typename... _Args>
   void pop(_T& value, _Args&&... __args) {
      assert(!empty());
      std::lock_guard<_Mutex> guard(m_mutex);
      internal_pop(value, std::forward<_Args>(__args)...);
   }

   /**
    * Discards the N topmost elements of the stack.
    *
    * PagedStack::discard(1) is equivalent to PagedStack::pop().
    * Discarding more elements than those currently in the stack has
    * undefined behavior.
    */
   void discard(size_t count) noexcept {
      assert(count <= size());
      std::lock_guard<_Mutex> guard(m_mutex);
      while(count >= m_curBase->size()) {
         count -= m_curBase->size();
         m_curBase->clear();
         if( m_curBase == m_base.begin() ) {
            return;
         }
         m_curBase--;
      }
      m_curBase->resize(m_curBase->size() - count);
   }

   /**
    * Discards all the items to the position indicated by the iterator.
    *
    * The element pointed by the iterator is the new stack top.
    */
   template<typename _TT, typename _IBase, typename _IData>
   void discard(const iterator_base<_TT, _IBase, _IData>& iter) noexcept {
      internal_discard(iter.m_iBase, iter.m_iData);
   }
   template<typename _TT, typename _IBase, typename _IData>
   void discard(const reverse_iterator_base<_TT, _IBase, _IData>& iter) noexcept {
      internal_discard(iter.m_iBase, iter.m_iData);
   }

   void clear() noexcept {
      //TODO: Swap base.
      std::lock_guard<_Mutex> guard(m_mutex);
      while(m_curBase != m_base.begin()) {
         m_curBase->clear();
         --m_curBase;
      }
   }

   /**
    * Resizes the amount of used memory to the minimum necessary.
    *
    * This method discards extra allocated pages, saving memory.
    * The topmost currently allocated page is not resized.
    */
   void shrink_to_fit() {
      std::lock_guard<_Mutex> guard(m_mutex);
      typename base_type::iterator iter = m_curBase;
      ++iter;
      // TODO: It would be nice to disengage the condemned elements and unlock.
      while(iter != m_base.end()) {
         iter = m_base.erase(iter);
      }
   }

   /**
    * Returns true if the stack is empty.
    *
    * The operation used to check the emptiness of the stack is simple, but not trivial.
    */
   bool empty() const noexcept {
      std::lock_guard<_Mutex> guard(m_mutex);
      return m_curBase == m_base.begin() && m_curBase->empty();
   }

   iterator begin() noexcept {std::lock_guard<_Mutex> guard(m_mutex); return iterator(m_curBase, --m_curBase->end(), m_base.begin());}
   const_iterator begin() const noexcept  {std::lock_guard<_Mutex> guard(m_mutex); return const_iterator(m_curBase, --m_curBase->end(), m_base.cbegin());}
   const_iterator cbegin() const noexcept {return begin();}

   iterator end() noexcept {std::lock_guard<_Mutex> guard(m_mutex); return iterator(m_base.begin(), --m_base.front().begin(), m_base.begin()); }
   const_iterator end() const noexcept {std::lock_guard<_Mutex> guard(m_mutex); return const_iterator(m_base.begin(), --m_base.front().begin(), m_base.cbegin()); }
   const_iterator cend() const noexcept {return end(); }

   reverse_iterator rbegin() noexcept {std::lock_guard<_Mutex> guard(m_mutex); return reverse_iterator(m_base.begin(), m_base.front().begin(), --m_base.end());}
   const_reverse_iterator rbegin() const noexcept {std::lock_guard<_Mutex> guard(m_mutex); return const_reverse_iterator(m_base.cbegin(), m_base.front().cbegin(), --m_base.cend());}
   const_reverse_iterator crbegin() const noexcept {return rbegin();}

   reverse_iterator rend() noexcept{std::lock_guard<_Mutex> guard(m_mutex); return reverse_iterator(m_curBase, m_curBase->end(), m_curBase);}
   const_reverse_iterator rend() const noexcept{std::lock_guard<_Mutex> guard(m_mutex); return const_reverse_iterator(m_curBase, m_curBase->end(), m_curBase);}
   const_reverse_iterator crend() const noexcept{return rend(); }

   sync_iterator sync_begin() noexcept {std::lock_guard<_Mutex> guard(m_mutex); return sync_iterator(this, m_curBase, --m_curBase->end(), m_base.begin());}
   sync_iterator sync_end() noexcept {std::lock_guard<_Mutex> guard(m_mutex); return sync_iterator(this, m_base.begin(), --m_base.front().begin(), m_base.begin());}
   const_sync_iterator sync_begin() const noexcept {std::lock_guard<_Mutex> guard(m_mutex); return const_sync_iterator(this, m_curBase, --m_curBase->cend(), m_base.cbegin());}
   const_sync_iterator sync_end() const noexcept {std::lock_guard<_Mutex> guard(m_mutex); return const_sync_iterator(this, m_base.begin(), --m_base.front().cbegin(), m_base.cbegin());}
   const_sync_iterator csync_begin() const noexcept {return sync_begin();}
   const_sync_iterator csync_end() const noexcept {return sync_end();}

private:

   reverse_iterator internal_from_top(size_t depth) noexcept {
      typename base_type::iterator iBase = m_curBase;
      while(depth > iBase->size()) {
         depth -= iBase->size();
         iBase--;
      }
      return reverse_iterator(iBase, iBase->begin() + (iBase->size() - depth), m_curBase);
   }


   const_reverse_iterator internal_from_top(size_t depth) const noexcept {
      typename base_type::iterator iBase = m_curBase;
      while(depth > iBase->size()) {
         depth -= iBase->size();
         iBase--;
      }
      return const_reverse_iterator(iBase, iBase->begin() + (iBase->size() - depth), m_curBase);
   }

public:

   /**
    * Return a reverse iterator from the nth- element to the top.
    */
   reverse_iterator from_top(size_t depth) noexcept {
      assert(depth <= size());
      std::lock_guard<_Mutex> guard(m_mutex);
      return internal_from_top(depth);
    }

   const_reverse_iterator from_top(size_t depth) const noexcept {
      assert(depth <= size());
      std::lock_guard<_Mutex> guard(m_mutex);
      return internal_from_top(depth);
   }

   /**
    * Read the topmost elements without popping them.
    *
    * The first parameter is read from top of the stack.
    */
   template<typename ... _Args>
   void peek(_Args&&... __args) const
   {
      assert(sizeof...(__args) <= size());
      std::lock_guard<_Mutex> guard(m_mutex);
      distribute(cbegin(), std::forward<_Args>(__args)...);
   }

   /**
    * Reads the parameters from the stack in FIFO order from a certain depth.
    *
    * The first parameter will be copied from depth items down the stack,
    * and the last one from the top of the stack.
    *
    * If depth is less than the size of the argument, the method has
    * undefined behaviour.
    */
   template<typename ... _Args>
   void peek_depth(size_t depth, _Args&&... __args) const
   {
      assert(depth <= size());
      assert(depth >= sizeof...(__args));
      std::lock_guard<_Mutex> guard(m_mutex);
      distribute(from_top(depth), std::forward<_Args>(__args)...);
   }

   /**
    * Reads the parameters from the stack in FIFO order.
    *
    * The first parameter will be copied from depth items down the stack,
    * and the last one from the top of the stack.
    */
   template<typename ... _Args>
   void peek_reverse(_Args&&... __args) const
   {
      assert(sizeof...(__args) <= size());
      std::lock_guard<_Mutex> guard(m_mutex);
      distribute(from_top(sizeof...(__args)), std::forward<_Args>(__args)...);
   }


   /**
    * Reads and discards the parameters from the stack in FIFO order.
    *
    * The first parameter will be copied from depth items down the stack,
    * and the last one from the top of the stack.
    *
    * After the operation, the top of the stack will be the element befor
    * the first parameter.
    */
   template<typename ... _Args>
   void pop_reverse(_Args&&... __args)
   {
      assert(sizeof...(__args) <= size());
      std::lock_guard<_Mutex> guard(m_mutex);
      auto pos = internal_from_top(sizeof...(__args));
      auto start = pos;
      distribute(pos, std::forward<_Args>(__args)...);
      discard(start);
   }

   /**
    * Computes the size of the stack.
    *
    * This is a lengthy operation. Don't use it on optimised code
    */

   size_t size() const noexcept {
      size_t count = 0;
      std::lock_guard<_Mutex> guard(m_mutex);
      auto iter = m_base.begin();
      while(iter != m_curBase) {
         count += iter->size();
         ++iter;
      }
      count += m_curBase->size();
      return count;
   }

   allocator_type get_allocator() const noexcept { return m_dataAllocator;}

   /**
    * Diagnostic
    */
   void getStats(size_t& blocks, size_t& depth, size_t& curBlock, size_t& curData) const noexcept {
      std::lock_guard<_Mutex> guard(m_mutex);
      blocks = m_base.size();
      depth = m_pageSize;
      typename base_type::const_iterator iter = m_base.begin();
      curBlock = 1;
      while(iter != m_curBase) {
         ++curBlock;
         ++iter;
      }
      curData = m_curBase->size();
   }
};


}

#endif /* _FALCON_PAGEDSTACK_H_ */

/* end of pagedstack.h */

