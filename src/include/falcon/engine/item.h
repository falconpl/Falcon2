/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: item.h

  Falcon Item
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 23 Feb 2019 04:28:03 +0000
  Touch : Sat, 23 Feb 2019 07:17:16 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_ITEM_H_
#define _FALCON_ITEM_H_

#include <setup.h>
#include <types.h>

#include <algorithm>
#include <atomic>
#include <vector>

namespace Falcon {

class IHandler;

class Item {
public:
	using sequence = std::vector<Item>;

	using Data = union __dt {
		bool asBool;
		char asChars[8];
		int asInt;
		int64 asInt64;
		double asDouble;
		void* asPointer;

		__dt():
			asPointer(nullptr)
		{}
		__dt(void* ptr):
			asPointer(ptr)
		{}

	};

	using ConceptData = struct {
		IHandler* concept{nullptr};
		Data data;
	};

	Item() noexcept: m_content{nullptr, nullptr} {}
	Item(const Item& other) noexcept {copy(other);}
	Item(Item&& other) noexcept:
		m_content(other.m_content.exchange({nullptr, nullptr}, std::memory_order_acq_rel))
	{}
	~Item() = default;

	void set(const ConceptData& cd) volatile noexcept {m_content.store(cd, std::memory_order_relaxed);}
	ConceptData get() const volatile noexcept { return m_content.load(std::memory_order_relaxed);}
	void copy(const Item& other) volatile {
		// todo. Color?
		m_content.store(
				other.m_content.load(std::memory_order_acquire),
				std::memory_order_release);
	}

	Item& operator=(const Item& other) volatile noexcept {copy(other); return *this;}

	bool isNil() const volatile noexcept {return get().concept == nullptr;}
	void setNil() volatile noexcept {set({nullptr, nullptr});}

	const IHandler& concept() const noexcept { return *get().concept; }
	IHandler& concept() noexcept { return *get().concept; }
	const Data& data() const noexcept { return get().data; }
	Data& data() noexcept { return get().data; }

private:
	using Content = std::atomic<ConceptData>;
	// many important constructors accept m_content as uninitialized.
	Content m_content;
};
}

#endif /* _FALCON_ITEM_H_ */

/* end of item.h */

