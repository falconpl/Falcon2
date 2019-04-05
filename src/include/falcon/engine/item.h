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
		m_content(other.get_weak())
	{}
	~Item() = default;

	void set(const ConceptData& cd) noexcept
	{
		fast_lock();
		m_content.data = cd.data;
		fast_unlock(cd.concept);
	}

	void set_weak(const ConceptData& cd) noexcept
	{
		m_content.concept.store(cd.concept, std::memory_order_relaxed);
		m_content.data = cd.data;
	}

	ConceptData get() const noexcept
	{
		ConceptData ret{fast_lock(), m_content.data};
		fast_unlock(ret.concept);
		return ret;
	}

	ConceptData get_weak() const noexcept
	{
		return {m_content.concept.load(std::memory_order_relaxed), m_content.data};
	}

	void copy(const Item& other) noexcept {
		// TODO Color?
		auto temp = other.get();
		fast_lock();
		m_content.data = temp.data;
		fast_unlock(temp.concept);
	}

	void copy_weak(const Item& other) noexcept {
		// TODO Color?
		set_weak(other.get_weak());
	}

	Item& operator=(const Item& other) noexcept {copy(other); return *this;}
	bool operator==(const Item& other) const noexcept {return get() == other.get();}
	bool operator!=(const Item& other) const noexcept {return ! (*this == other);}
	bool fast_compare(const Item& other) const noexcept {return get_weak() == other.get_weak();}

	bool isNil() const noexcept {return get().concept == nullptr;}
	void setNil() noexcept {set({nullptr, nullptr});}

	const IHandler& concept() const noexcept { return *get().concept; }
	IHandler& concept() noexcept { return *get().concept; }
	const Data& data() const noexcept { return get().data; }
	Data& data() noexcept { return get().data; }

	template<typename _T>
	void getAs(_T& target) const { getAs_mode(target, get); }
	template<typename _T>
	void getAs_weak(_T& target) const { getAs_mode(target, get_weak); }

	// TODO set from integral/well known types requires the engine to be in place.

private:
	using Content = struct {
		std::atomic<IHandler*> concept{nullptr};
		Data data;
	};

	mutable Content m_content;
	IHandler* fast_lock() const volatile noexcept {
		IHandler* cpt;
		while(true) {
			// wait for the entity to be valid (and unlocked)
			while((cpt = m_content.concept.load()) == reinterpret_cast<IHandler*>(-1)) {}
			// hope we can get it
			if( m_content.concept.compare_exchange_weak(cpt, reinterpret_cast<IHandler*>(-1), std::memory_order_acquire) )
			{
				// if we can null it, return the old value.
				return cpt;
			}
		}
	}

	void fast_unlock(IHandler *value) const volatile noexcept {
		m_content.concept.store(value, std::memory_order_release);
	}

	template<typename _T, typename _Getter>
	void getAs_mode(_T& target ) const {
		auto cd = _Getter();
		if(cd.concept) {
			cd.concept->getAs(cd.data, target);
		}
	}

};
}

#endif /* _FALCON_ITEM_H_ */

/* end of item.h */

