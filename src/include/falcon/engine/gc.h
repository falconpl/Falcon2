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

#include <atomic>
#include <memory>
#include <type_traits>


#ifndef _FALCON_GC_H_
#define _FALCON_GC_H_

namespace Falcon {

class GarbageCollector
{
public:
	GarbageCollector(){}
	~GarbageCollector(){
		// At descrution, we don't clear the rings: we just free the arena.
	}

	template<typename _T>
	struct traits {
		using pointer = _T*;
		using reference = _T&;
		using const_pointer = const _T*;
		using const_reference = const _T&;
	};

	using pointer = traits<void *>::pointer;
	using Deletor = void(*)(pointer);
	using Marker = void(*)(pointer);
	pointer getMemory(size_t size, Deletor deletor=nullptr, Marker marker=nullptr)
	{
		AlignedEntry* ring = createEntry(size, deletor, marker);
		return reinterpret_cast<pointer>(ring+1);
	}

	template<typename _T> traits<_T>::pointer getData(size_t count, Deletor deletor=nullptr, Marker marker=nullptr)
	{
		return static_cast<traits<_T>::pointer(getMemory(count*sizeof(_T), deletor, marker));
	}

	bool isGC(pointer data){
		// test.
		return true;
	}

private:

	using mark_color_t = short int;
	size_t m_allocated{0};
	size_t m_allocatedBlocks{0};
	mark_color_t m_blackMark{2};

	mark_color_t whiteMark() const {return m_blackMark == 0 ? 2 : 0;}
	mark_color_t grayMark() const {return 1;}
	mark_color_t blackMark() const {return m_blackMark; }

	struct GCEntry {
		size_t m_size{0};
		bool m_locked{false};
		mark_color_t m_mark{0};
		GCEntry* m_next{this};
		GCEntry* m_prev{this};
		Deletor m_deletor{nullptr};
		Marker m_marker{nullptr};

		GCEntry(size_t size, Deletor deletor, Marker marker, bool locked=false):
			m_size(size), m_deletor(deletor), m_marker(marker), m_locked(locked)
		{}
		GCEntry() = default;
		GCEntry(const GCEntry&) = delete;
		GCEntry(GCEntry&&) = delete;

		void insert(GCEntry* prev, GCEntry* next) {
			prev->m_next = this;
			next->m_prev = this;
			this->m_next = next;
			this->m_prev = prev;
		}

		void chain(GCEntry* new_prev) {
			// currently, the last element of the ring points to this.
			// -- link it with the next of the other ring's element.
			m_prev->m_next = new_prev->m_next;
			m_prev->m_next->m_prev = m_prev;

			// Now link the head of this chain with the other ring's head.
			new_prev->m_next = this;
			m_prev = new_prev;
		}

		void extract() {
			m_prev->m_next = m_next;
			m_next->m_prev = m_prev;
			m_next = m_prev = this;
		}

		bool empty() const noexcept {
			return m_next == this;
		}

		size_t size() const noexcept {
			return m_size ;
		}

		void lock() const noexcept {
			m_locked = true;
		}

		void unlock() const noexcept {
			m_locked = false;
		}

		mark_color_t mark() const noexcept {
			return m_mark;
		}

		void mark(mark_color_t m) noexcept {
			return m_mark = m;
		}
	};

	using AlignedEntry = std::aligned_storage<sizeof(GCEntry), alignof(GCEntry)>::type;

	mutable std::atomic<bool> m_ringLock{false};
	// Store the rings locally, so we can easily get rid of them.
	AlignedEntry m_ring1;
	AlignedEntry m_ring2;
	AlignedEntry m_ring3;

	// use pointers
	GCEntry* m_whiteRing{&m_ring3};
	GCEntry* m_grayRing{&m_ring3};
	GCEntry* m_blackRing{&m_ring3};



	void lockRings() const {
		bool value = false;
		while(!m_ringLock.compare_exchange_weak(value, true));
	}

	void unlockRings() const {
		m_ringLock = false;
	}

	AlignedEntry* createEntry(size_t size, Deletor deletor, Marker marker)
	{
		// for a test, use new/delete
		AlignedEntry* entry = allocEntry(size, deletor, marker);
		entry->mark(grayMark());
		lockRings();
		m_allocated += size;
		m_allocatedBlocks += 1;
		entry->chain(m_grayRing);
		unlockRings();
		return entry;
	}

	void releaseEntry(AlignedEntry* entry) {
		lockRings();
		m_allocated -= entry->m_size;
		m_allocatedBlocks -= 1;
		entry->extract();
		unlockRings();
		recycleEntry(entry);
	}

	// This plugs directly in the memory manager.
	AlignedEntry* allocEntry(size_t size, Deletor deletor, Marker marker) {
		return new AlignedEntry(size, deletor, marker);
	}

	// This plugs directly in the memory manager.
	void recycleEntry(AlignedEntry* entry) {
		//  give the entry an opportunity to self-destruct
		if(entry->m_deletor) {
			entry->m_deletor(entrty + 1);
		}
		// todo: arena->recycle
		delete entry;
	}

	// called at the end of the collection loop.
	void swapRings() {
		lockRings();
		std::swap(m_whiteRing, m_blackRing);
		unlockRings();
		// GC is the only user of the mark byte.
		m_blackMark == 0 ? 2 : 0;
	}

	void collect() {
		collect(m_whiteRing);
	}

	// Exposing a collect that can work on any ring, in order to implement
	// a debug clear-all-at-end.
	// -- Normally, at end we just dispose of the allocation arena.
	void collect(AlignedEntry* root) {
		// only the gray and the black rings are shared; white ring is for GC only.
		AlignedEntry *current = root->m_next;
		while(current != root) {
			AlignedEntry* old = current;
			current = current->m_next;
			recycleEntry(old);
		}

		// clear the white ring
		root->m_prev = m_whiteRing->m_next = m_whiteRing;
	}


	void moveToRing(mark_color_t color, AlignedEntry* entry, pointer data)
	{
		GCEntry* entry = static_cast<AlignedEntry*>(data)-1;
		if(entry->mark() != color) {
			lockRings();
			entry->extract();
			entry->chain(m_grayRing);
			unlockRings();
			entry->mark(color);
		}
	}

	void moveToGray(pointer data) {moveToRing(grayMark(), m_grayRing, data);}
	void moveToBlack(pointer data) {moveToRing(grayMark(), m_grayRing, data);}
};

}

#endif /* _FALCON_GC_H_ */

/* end of gc.h */
