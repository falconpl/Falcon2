/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: deepstack.h

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
#include <cassert>
#include <memory>
#include <falcon/engine/distribute.h>

#ifndef _FALCON_DEEPSTACK_H_
#define _FALCON_DEEPSTACK_H_

namespace Falcon {

/**
 * Stack-like structure indefinitely growable.
 *
 */

template<typename _T,
			class _Allocator=std::allocator<_T>,
			typename _DeepAlloc=std::allocator<std::vector<_T, _Allocator>> >
class DeepStack
{
private:
	using VData = std::vector<_T, _Allocator>;
	using VBase = std::vector<std::unique_ptr<VData>, _DeepAlloc >;

	VBase m_base;
	size_t m_pageSize;
	typename VBase::iterator m_curBase;

	friend class iterator;

	void growBase() {
		size_t size = m_base.size();
		m_base.resize(size == 0 ? 1: size * 2);
		for(size_t p = size; p < m_base.size(); ++p) {
			m_base[p].reset(new VData());
			m_base[p]->reserve(m_pageSize);
		}
		m_curBase = m_base.begin() + size;
	}


	void advance() {
		if ((*m_curBase)->size() == m_pageSize) {
			++m_curBase;
			if(m_curBase == m_base.end()) {
				growBase();
			}
		}
	}


	template<typename _IBase>
	static typename VData::iterator depth_from_top(size_t depth, _IBase& iBase) noexcept
	{
		while(depth > (*iBase)->size()) {
			depth -= (*iBase)->size();
			iBase--;
		}
		return (*iBase)->begin() + ((*iBase)->size() - depth);
	}

	template<typename... _Args>
	void internal_pop(_T& value, _Args&&... __args) {
		value = (*m_curBase)->back();
		internal_pop_one();
		internal_pop(std::forward<_Args>(__args)...);
	}

	void internal_pop_one() {
		(*m_curBase)->pop_back();
		if((*m_curBase)->empty() && m_curBase != m_base.begin()) {
			--m_curBase;
		}
	}

	void internal_pop() {}

	template<typename _IBase, typename _IData>
	void internal_discard(_IBase iBase, _IData iData)
	{
		while(m_curBase != iBase) {
			(*m_curBase)->clear();
			--m_curBase;
		}
		if(iData == (*m_curBase)->begin())
		{
			if (m_curBase != m_base.begin())
			{
				m_curBase--;
			}
			else {
				(*m_curBase)->clear();
			}
		}
		else {
			(*m_curBase)->resize(iData - (*m_curBase)->begin());
		}
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
			if(m_iData == (*m_iBase)->begin()) {
				--m_iBase;
				m_iData = (*m_iBase)->end();
			}
			--m_iData;
			return *this;
		}

		const reverse_iterator_base& operator++() noexcept {
			++m_iData;
			if(m_iData == (*m_iBase)->end() && m_iBase != m_iBaseEnd) {
				++m_iBase;
				m_iData = (*m_iBase)->begin();
			}
			return *this;
		}

	private:
		reverse_iterator_base(const _IBase& iBase,
				size_t depth,
				const _IBase& end) noexcept:
				m_iBase(iBase),
				m_iData(DeepStack::depth_from_top(depth, m_iBase)),
				m_iBaseEnd(end)
			{}

		reverse_iterator_base(const _IBase& iBase,
				 const _IData& iData,
				 const _IBase& end) noexcept:
			m_iBase(iBase),
			m_iData(iData),
			m_iBaseEnd(end)
		{}

		friend class DeepStack;

		_IBase m_iBase;
		_IData m_iData;
		_IBase m_iBaseEnd;
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
			if(m_iData == (*m_iBase)->begin() && m_iBase != m_iBaseBegin) {
				--m_iBase;
				m_iData = (*m_iBase)->end();
			}
			--m_iData;
			return *this;
		}

		const iterator_base& operator--() noexcept {
			++m_iData;
			if(m_iData == (*m_iBase)->end()) {
				++m_iBase;
				m_iData = (*m_iBase)->begin();
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

		friend class DeepStack;

		_IBase m_iBase;
		_IData m_iData;
		_IBase m_iBaseBegin;
	};

public:
	enum {
		DEFAULT_PAGE_SIZE=16,
		DEFAULT_BASE_SIZE=2
	};

	using value_type = _T;
	using reference_type = _T&;
	using iterator = iterator_base<_T, typename VBase::iterator, typename VData::iterator>;
	using const_iterator = iterator_base<const _T, typename VBase::const_iterator, typename VData::const_iterator>;
	using reverse_iterator = reverse_iterator_base<_T, typename VBase::iterator, typename VData::iterator>;
	using const_reverse_iterator = reverse_iterator_base<const _T, typename VBase::const_iterator, typename VData::const_iterator>;
	using allocator_type = typename VBase::allocator_type;

	DeepStack(size_t pageSize = DEFAULT_PAGE_SIZE, size_t prealloc = DEFAULT_BASE_SIZE):
		m_pageSize(pageSize)
	{
		m_base.resize(prealloc);
		for(auto& vdata: m_base) {
			vdata.reset(new VData());
			vdata->reserve(pageSize);
		}
		m_curBase = m_base.begin();
	}

	_T& top() noexcept { return (*m_curBase)->back(); }
	const _T& top() const noexcept { return (*m_curBase)->back(); }

	void push(const _T& data) {
		advance();
		(*m_curBase)->push_back(data);
	}

	/**
	 * Push multiple values at the end.
	 *
	 * The last parameter will be on top of the stack.
	 */
	template<typename... _Args>
	void push(const _T& data, _Args&&... __args) {
		advance();
		(*m_curBase)->push_back(data);
		push(std::forward<_Args>(__args)...);
	}

	/**
	 * Constructs an entry and places it on top of the stack.
	 */
	template<typename... _Args>
	void push_emplace(_Args&&... __args)	{
		advance();
		// vector::emplace writes at the previous iterator.
		(*m_curBase)->emplace_back(std::forward<_Args>(__args)...);
	}

	/**
	 * Removes the last element from the stack.
	 *
	 * Popping on an empty stack is an undefined behaviour.
	 */
	void pop() {
		assert(!empty());
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
		internal_pop(value, std::forward<_Args>(__args)...);
	}

	/**
	 * Discards the N topmost elements of the stack.
	 *
	 * DeepStack::discard(1) is equivalent to DeepStack::pop().
	 * Discarding more elements than those currently in the stack has
	 * undefined behavior.
	 */
	void discard(size_t count) noexcept {
		assert(count <= size());
		while(count >= (*m_curBase)->size()) {
			count -= (*m_curBase)->size();
			(*m_curBase)->clear();
			if( m_curBase == m_base.begin() ) {
				return;
			}
			m_curBase--;
		}
		(*m_curBase)->resize((*m_curBase)->size() - count);
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
		while(m_curBase != m_base.begin()) {
			(*m_curBase)->clear();
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
		size_t curPos = m_curBase - m_base.begin();
		m_base.resize(curPos+1);
		m_base.shrink_to_fit();
		m_curBase = m_base.begin() + curPos;
	}

	/**
	 * Returns true if the stack is empty.
	 *
	 * The operation used to check the emptiness of the stack is simple, but not trivial.
	 */
	bool empty() const noexcept {
		return m_curBase == m_base.begin() && (*m_curBase)->empty();
	}

	iterator begin() noexcept { return iterator(m_curBase, (*m_curBase)->end()-1, m_base.begin());}
	const_iterator begin() const noexcept  { return const_iterator(m_curBase, (*m_curBase)->end()-1, m_base.cbegin());}
	const_iterator cbegin() const noexcept { return begin();}

	iterator end() noexcept { return iterator(m_base.begin(), m_base.front()->begin()-1, m_base.begin()); }
	const_iterator end() const noexcept { return const_iterator(m_base.begin(), m_base.front()->begin()-1, m_base.cbegin()); }
	const_iterator cend() const noexcept { return end(); }

	reverse_iterator rbegin() noexcept {return reverse_iterator(m_base.begin(), m_base.front()->begin(), m_base.end()-1);}
	const_reverse_iterator rbegin() const noexcept {return const_reverse_iterator(m_base.cbegin(), m_base.front()->cbegin(), m_base.cend()-1);}
	const_reverse_iterator crbegin() const noexcept {return rbegin();}

	reverse_iterator rend() noexcept{ return reverse_iterator(m_curBase, (*m_curBase)->end(), m_curBase);}
	const_reverse_iterator rend() const noexcept{ return const_reverse_iterator(m_curBase, (*m_curBase)->end(), m_curBase);}
	const_reverse_iterator crend() const noexcept{ return rend(); }

	/**
	 * Return a reverse iterator from the nth- element to the top.
	 */
	reverse_iterator from_top(size_t depth) noexcept {
		assert(depth <= size());
		return reverse_iterator(m_curBase, depth, m_curBase);
	}

	const_reverse_iterator from_top(size_t depth) const noexcept {
		assert(depth <= size());
		return const_reverse_iterator(m_curBase, depth, m_curBase);
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
		auto pos = from_top(sizeof...(__args));
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
		auto iter = m_base.begin();
		while(iter != m_curBase) {
			count += (*iter)->size();
			++iter;
		}
		count += (*m_curBase)->size();
		return count;
	}


	/**
	 * Diagnostic
	 */
	void getStats(size_t& blocks, size_t& depth, size_t& curBlock, size_t& curData) const noexcept {
		blocks = m_base.size();
		depth = m_pageSize;
		curBlock = m_curBase - m_base.begin()+1;
		curData = (*m_curBase)->size();
	}


};


}

#endif /* _FALCON_DEEPSTACK_H_ */

/* end of deepstack.h */

