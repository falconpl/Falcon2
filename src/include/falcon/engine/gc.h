/*****************************************************************************
 FALCON2 - The Falcon Programming Language
 FILE: gc.h

 Garbage Collector
 -------------------------------------------------------------------
 Author: Giancarlo Niccolai
 Begin : Thu, 11 Apr 2019 08:02:58 +0100
 Touch : Thu, 11 Apr 2019 08:02:58 +0100

 -------------------------------------------------------------------
 (C) Copyright 2019 The Falcon Programming Language
 Released under Apache 2.0 License.
 ******************************************************************************/


#include <cassert>
#include <atomic>
#include <memory>
#include <mutex>
#include <falcon/engine/syncqueue.h>
#include <type_traits>
#include <falcon/futex.h>
#include <variant>
#include <vector>
#include <thread>
#include <future>


#ifndef _FALCON_GC_H_
#define _FALCON_GC_H_

namespace Falcon {

class GarbageCollector {
public:
   /** Creates a garbage collector ready to run.
    *
    * At creation, the collection thread is not started; it must be independently
    * started using the start() method, or passing the start paramter as true
    */
   GarbageCollector(bool start=false);
   ~GarbageCollector();
   GarbageCollector(GarbageCollector&& )=delete;
   GarbageCollector(const GarbageCollector&)=delete;


   /**
    * An ABC that is associated with created memory.
    *
    * To avoid double dereference in the virtual call,
    * do not use data members in sub-classes (just use
    * the handled items themselves to keep track of any
    * needed status information).
    */
   class Handler {
   public:
      virtual ~Handler() {
      }
      ;
      virtual void destroy(void* target) noexcept=0;
      virtual void mark(void* target, GarbageCollector* owner) noexcept=0;
      virtual void relocate(void* source, void *target) noexcept=0;

   };

   struct Stats {
      size_t all_blocks;
      size_t all_allocated;
      size_t white_blocks;
      size_t white_allocated;
      size_t gray_blocks;
      size_t gray_allocated;
      size_t black_blocks;
      size_t black_allocated;
   };

   Stats getStats() const {
      Stats s;
      std::lock_guard guard(m_ringLock);
      s.all_blocks = m_ringStats[e_no_mark].blocks;
      s.all_allocated = m_ringStats[e_no_mark].allocated;
      s.white_blocks = m_ringStats[e_white_mark].blocks;
      s.white_allocated = m_ringStats[e_white_mark].allocated;
      s.gray_blocks = m_ringStats[e_gray_mark].blocks;
      s.gray_allocated = m_ringStats[e_gray_mark].allocated;
      s.black_blocks = m_ringStats[e_black_mark].blocks;
      s.black_allocated = m_ringStats[e_black_mark].allocated;
      return s;
   }

   void mark(void* data) const noexcept {
      GCEntry* entry = static_cast<GCEntry*>(data);
      entry--;
      moveToGray(entry);
   }

   /** Start the garbage collector main thread.
    *
    * This thread is responsible to:
    * - perform marking.
    * - periodically perform incremental collection.
    * - periodically issue a stop-all request to mutators and perform a full collection.
    */
   bool start();

   /** Stops the main thread.
    *
    * This method blocks until the main thread is completed.
    * After calling stop(), it is safe to invoke start again.
    */
   bool stop();

   /** Starts a collection loop (as soon as possible).
    *
    * Returns a future that will get the value of collected memory
    * once the operation is complete.
    */
   std::future<size_t> forceCollection() noexcept;

private:

   //===========================================================
   // Main thread
   void run();

   // TODO: Use a engine-specific implementation of threads with ID/names.
   std::thread m_mainThread;
   std::atomic<bool> m_isStarted{false};
   bool m_isStopped;

   struct StopMessage{};
   struct CollectMessage{
      std::shared_ptr<std::promise<size_t>> collected;
      CollectMessage(): collected(std::make_shared<std::promise<size_t>>()) {}
      CollectMessage(const CollectMessage& other): collected(other.collected) {}
   };

   using Message = std::variant<
         StopMessage,
         CollectMessage
   >;
   sync_queue<Message> m_messageQueue;

   class Receiver {
   public:
      Receiver(GarbageCollector* owner): m_owner(owner) {}
      void operator() (StopMessage&);
   private:
      GarbageCollector* m_owner;
   };
   friend class Receiver;
   //===========================================================


   enum {
      SPINLOCK_COUNT=30
   };
   friend class Grabber;

   mutable Futex<SPINLOCK_COUNT> m_mtxHandlers;
   std::vector<std::unique_ptr<Handler>> m_handlers;

   typedef enum {
      e_no_mark = 0,
      e_white_mark = 1,
      e_gray_mark = 2,
      e_black_mark = 3
   } mark_color_t;

   struct StatEntry {
      size_t allocated;
      size_t blocks;
   };
   mutable std::vector<StatEntry> m_ringStats { { 0, 0 }, { 0, 0 }, { 0, 0 }, {0, 0 } };

   struct GCEntry {
      GCEntry* m_next { this };
      GCEntry* m_prev { this };
      Handler* m_handler { nullptr };
      unsigned int m_size { 0 };
      std::atomic<bool> m_locked { false };
      char m_mark { e_no_mark };

      //Entries are born locked, and get unlocked as the owner assigns them.
      GCEntry(size_t size, Handler* handler = nullptr, bool locked = true)
            noexcept :
            m_size(size), m_handler(handler), m_locked(locked) {
      }

      GCEntry() = default;
      GCEntry(const GCEntry& other) = default;
      GCEntry(GCEntry&&) = default;

      void insert(GCEntry* prev, GCEntry* next) noexcept {
         prev->m_next = this;
         next->m_prev = this;
         this->m_next = next;
         this->m_prev = prev;
      }

      /** Moves the whole ring (one element or more) inside a new one.
       *
       */
      void chain(GCEntry* new_prev) noexcept {
         // currently, the last element of the ring points to this.
         // -- link it with the next of the other ring's element.
         m_prev->m_next = new_prev->m_next;
         m_prev->m_next->m_prev = m_prev;

         // Now link the head of this chain with the other ring's head.
         new_prev->m_next = this;
         m_prev = new_prev;
      }

      /**
       * Move all the chain AROUND this entry to the target ring.
       */
      void transfer(GCEntry* target) noexcept {
         // create a ring extracting this.
         GCEntry* head = m_next;
         extract();
         // chain the previous head to target
         head->chain(target);
      }

      /**
       * Reclaim all the nodes AROUND this node.
       */
      void clear(GarbageCollector* owner) {
         GCEntry* root = m_next;
         while (root != this) {
            GCEntry* old = root;
            root = root->m_next;
            owner->recycleEntry(old);
         }
      }

      void extract() noexcept {
         m_prev->m_next = m_next;
         m_next->m_prev = m_prev;
         m_next = m_prev = this;
      }

      bool empty() const noexcept {
         return m_next == this;
      }

      size_t size() const noexcept {
         return m_size;
      }

      void lock() noexcept {
         m_locked.store(true, std::memory_order_release);
      }

      void unlock() noexcept {
         m_locked.store(false, std::memory_order_release);
      }

      bool isLocked() const noexcept {
         return m_locked.load(std::memory_order_acquire);
      }

      mark_color_t mark() const noexcept {
         return static_cast<mark_color_t>(m_mark);
      }

      void mark(mark_color_t m) noexcept {
         m_mark = m;
      }
   };

   // We want GCEntry to be within a block size
   static_assert(sizeof(GCEntry) <= 0x20);

   mutable Futex<SPINLOCK_COUNT> m_ringLock;
   // Store the rings locally, so we can easily get rid of them.
   GCEntry m_ring1;
   GCEntry m_ring2;
   GCEntry m_ring3;

   // use pointers
   GCEntry* m_whiteRing { &m_ring1 };
   GCEntry* m_grayRing { &m_ring2 };
   GCEntry* m_blackRing { &m_ring3 };

   GCEntry* createEntry(size_t size, Handler* handler = nullptr) {
      // for a test, use new/delete
      GCEntry* entry = allocEntry(size, handler);
      entry->mark(e_gray_mark);

      std::lock_guard guard(m_ringLock);
      m_ringStats[e_no_mark].allocated += size;
      m_ringStats[e_no_mark].blocks += 1;
      m_ringStats[e_gray_mark].allocated += size;
      m_ringStats[e_gray_mark].blocks += 1;
      entry->chain(m_grayRing);
      return entry;
   }

   void releaseEntry(GCEntry* entry) {
      std::lock_guard guard(m_ringLock);
      m_ringStats[e_no_mark].allocated -= entry->m_size;
      m_ringStats[e_no_mark].blocks -= 1;
      m_ringStats[entry->mark()].allocated -= entry->m_size;
      m_ringStats[entry->mark()].blocks -= 1;
      entry->extract();
      recycleEntry(entry);
   }

   // This plugs directly in the memory manager.
   GCEntry* allocEntry(size_t size, Handler* handler) {
      void *data = ::operator new(size + sizeof(GCEntry));
      return new (data) GCEntry(size, handler);
   }

   // This plugs directly in the memory manager.
   void recycleEntry(GCEntry* entry) {
      //  give the entry an opportunity to self-destruct
      if (entry->m_handler) {
         entry->m_handler->destroy(entry + 1);
      }

      // TODO: arena->recycle
      entry->~GCEntry();
      ::operator delete(entry, entry->m_size + sizeof(GCEntry));
   }

   /**
    * Move all the black entries to the white ring.
    *
    * This marks the beginning of a new scan.
    */
   void swapRings() {
      std::lock_guard guard(m_ringLock);
      m_blackRing->transfer(m_whiteRing);
      m_ringStats[e_white_mark].allocated +=
            m_ringStats[e_black_mark].allocated;
      m_ringStats[e_white_mark].blocks += m_ringStats[e_black_mark].blocks;
      m_ringStats[e_black_mark].allocated = 0;
      m_ringStats[e_black_mark].blocks = 0;
   }

   void collect() {
      // GC is the only user of the white ring (and its stats).
      collect(m_whiteRing);
      // but not of its stats.
      std::lock_guard guard(m_ringLock);
      m_ringStats[e_white_mark].allocated = 0;
      m_ringStats[e_white_mark].blocks += 0;
   }

   // Exposing a collect that can work on any ring, in order to implement
   // a debug clear-all-at-end.
   // -- Normally, at end we just dispose of the allocation arena.
   void collect(GCEntry* root) {
      // only the gray and the black rings are shared; white ring is for GC only.
      GCEntry *current = root->m_next;
      while (current != root) {
         GCEntry* old = current;
         current = current->m_next;
         recycleEntry(old);
      }

      // clear the owner ring
      root->m_prev = m_whiteRing->m_next = m_whiteRing;
   }

   void moveToRing(mark_color_t color, GCEntry* ring, void* data) const noexcept
   {
      GCEntry* nentry = static_cast<GCEntry*>(data) - 1;
      if (nentry->mark() != color) {
         // lock
         std::lock_guard guard(m_ringLock);
         // account
         m_ringStats[color].allocated += nentry->size();
         m_ringStats[color].blocks += 1;
         m_ringStats[nentry->mark()].allocated -= nentry->size();
         m_ringStats[nentry->mark()].blocks -= 1;
         // swap
         nentry->extract();
         nentry->chain(ring);
         nentry->mark(color);
      }
   }

   void moveToGray(void* data) const noexcept {
      moveToRing(e_gray_mark, m_grayRing, data);
   }
   void moveToBlack(void* data) const noexcept {
      moveToRing(e_black_mark, m_blackRing, data);
   }

public:

   /**
    * Safe staged allocator for objects.
    *
    * Allocation in the garbage collector are performed through the agency
    * of this class. The grabber stores all the created entities
    * atomically in the garbage collector; this allow the construction of complex,
    * and their correct initialisation, in a safe area, protected the collector
    * inspection and activity, until the objects are coherently constructed.
    *
    * Normally, the grabber will commit all the allocated blocks to the garbage
    * collector at its destruction, unless the isSafe parameter of its constructor
    * is false.
    *
    * In non-safe mode, the method commit() must be explicitly
    * called, or the destructor of the Grabber will abort the program (in debug),
    * or discard all the information generated (in release code).
    *
    * This is to prevent partially constructed objects (for example after throwing
    * and exception) to reach the garbage collector, where it would cause
    * undefined behaviour later on, during scan or collection, that would be
    * extremely hard to trace back to the original cause.
    *
    * In case the constructed objects are coherent at each step of the creation,
    * or exceptions are correctly handled, the grabber can be created in safe mode,
    * and the commit() method doesn't need to be explicitly called.
    *
    * Requesting further allocation after having invoked commit will assert, or
    * an exception will be thrown.
    *
    */
   class Grabber {
   public:
      Grabber() = delete;
      Grabber(GarbageCollector& owner, bool isSafe = true) :
            m_owner(&owner), m_safe(isSafe) {
      }
      Grabber(const Grabber&) = delete;
      Grabber(Grabber&& other) :
            m_owner(std::move(other.m_owner)), m_safe(other.m_safe), m_allocated(
                  other.m_allocated), m_blocks(other.m_blocks) {
         assert(!other.m_committed);
         // TODO: Throw a personalised exception
         if (other.m_committed) {
            throw std::runtime_error(
                  "Grabber: can't move from a committed grabber");
         }
         other.m_ringRoot.transfer(&m_ringRoot);
      }

      ~Grabber() {
         if (m_safe) {
            commit();
         } else {
            assert(m_committed);
            m_ringRoot.clear(m_owner);
         }
      }

      /**
       * Get raw memory from the garbae collector
       */
      void *getMemory(size_t size, Handler* handler = nullptr) {
         // TODO: Throw a personalised exception
         if (m_committed) {
            throw std::runtime_error("Grabber: allocation after commit");
         }
         GCEntry* ring = m_owner->allocEntry(size, handler);
         m_allocated += size;
         ++m_blocks;
         return reinterpret_cast<void *>(ring + 1);
      }

      /**
       * Get a typed storage correctly sized to store a certain count of items.
       */
      template<typename _TT>
      auto getDataBuffer(size_t count, Handler* memoryHandler = nullptr) {
         return static_cast<_TT *>(getMemory(count * sizeof(_TT), memoryHandler));
      }

      /**
       * Get a typed pointer.
       */
      template<typename _TT>
      auto getDataMemory(Handler* memoryHandler = nullptr) {
         return static_cast<_TT*>(getMemory(sizeof(_TT), memoryHandler));
      }

      void commit() {
         if (!m_committed) {
            m_committed = true;

            std::lock_guard guard(m_owner->m_ringLock);
            m_ringRoot.transfer(m_owner->m_grayRing);
            m_owner->m_ringStats[e_no_mark].allocated += m_allocated;
            m_owner->m_ringStats[e_no_mark].blocks += m_blocks;
            m_owner->m_ringStats[e_gray_mark].allocated += m_allocated;
            m_owner->m_ringStats[e_gray_mark].blocks += m_blocks;
         }
      }

   private:
      GarbageCollector* m_owner;
      bool m_committed { false };
      bool m_safe;
      GCEntry m_ringRoot;
      size_t m_allocated { 0 };
      unsigned int m_blocks { 0 };
   };
};

}

#endif /* _FALCON_GC_H_ */

/* end of gc.h */
