/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: deepstack.fut.cpp

  Test for DeepStack
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 06 Apr 2019 16:17:19 +0100
  Touch : Sat, 06 Apr 2019 16:17:19 +0100

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <falcon/engine/deepstack.h>
#include <variant>
#include <functional>
#include <sstream>

#include <iostream>

class DeepStackTest: public Falcon::testing::TestCase
{
public:
	struct SomeStruct {
		int alpha{0};
		std::string beta;

		SomeStruct()=default;
		SomeStruct(int a, const std::string& b):
			alpha(a),
			beta(b)
		{}
	};

	using datatype = std::variant<int, std::string, SomeStruct>;
	using DeepStack = Falcon::DeepStack<datatype>;
	DeepStack m_stack{4,2};

	void SetUp() {}
	void TearDown() {}

	void check_stats(size_t cb, size_t cd) {
		size_t blocks, depth, curBlock, curData;
		m_stack.getStats(blocks, depth, curBlock, curData);

		int power = 2;
		while(power < cb)
		    power*=2;

		EXPECT_EQ(power, blocks);
		EXPECT_EQ(4, depth);
		EXPECT_EQ(cb, curBlock);
		EXPECT_EQ(cd, curData);
	}


	template<typename _Iter>
	void check_iterator(int from, int to, std::function<int(int)> update,  std::function<int(int)> rupdate, _Iter begin, _Iter end)
	{
		// don't use auto, we want to know the iterator is const
		int i = from;
		for(_Iter iter = begin; iter != end; ++iter)
		{
			EXPECT_EQ(i, std::get<int>(*iter));
			i = update(i);
		}
		EXPECT_EQ(update(to), i);

		i = from;
		for(_Iter iter = begin; iter != end; iter++)
		{
			EXPECT_EQ(i, std::get<int>(*iter));
			i = update(i);
		}
		EXPECT_EQ(update(to), i);

		auto eiter = end;
		--eiter;
		eiter--;
		EXPECT_EQ(rupdate(to), std::get<int>(*eiter));
		++eiter;
		EXPECT_EQ(to, std::get<int>(*eiter));
	}

};

TEST_F(DeepStackTest, smoke)
{
	EXPECT_TRUE(m_stack.empty());
	EXPECT_EQ(0, m_stack.size());
}

TEST_F(DeepStackTest, simple_push)
{
	m_stack.push("Hello world");

	EXPECT_FALSE(m_stack.empty());
	EXPECT_EQ(1, m_stack.size());
	EXPECT_STREQ("Hello world", std::get<std::string>(m_stack.top()));
}

TEST_F(DeepStackTest, double_push)
{
	m_stack.push(1, "Hello world");

	EXPECT_FALSE(m_stack.empty());
	EXPECT_EQ(2, m_stack.size());
	EXPECT_STREQ("Hello world", std::get<std::string>(m_stack.top()));
}


TEST_F(DeepStackTest, simple_emplace)
{
	Falcon::DeepStack<SomeStruct> emplacer;

	emplacer.push_emplace(1, "Hello world");

	EXPECT_FALSE(emplacer.empty());
	EXPECT_EQ(1, emplacer.size());
	const SomeStruct& top = emplacer.top();

	EXPECT_EQ(1, top.alpha );
	EXPECT_STREQ("Hello world", top.beta);
}

TEST_F(DeepStackTest, double_emplace)
{
	Falcon::DeepStack<SomeStruct> emplacer;

	emplacer.push_emplace(0, "Hello world");
	emplacer.push_emplace(1, "Hello again");

	EXPECT_FALSE(emplacer.empty());
	EXPECT_EQ(2, emplacer.size());
	const SomeStruct& top = emplacer.top();

	EXPECT_EQ(1, top.alpha );
	EXPECT_STREQ("Hello again", top.beta);

}

TEST_F(DeepStackTest, fill_one_buffer)
{
	m_stack.push(1, 2, 3, "top");
	EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));
	check_stats(1, 4);
}


TEST_F(DeepStackTest, pure_pop)
{
	m_stack.push("top");
	m_stack.push("discarded");
	m_stack.pop();
	EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));
	check_stats(1, 1);
}

TEST_F(DeepStackTest, pure_pop_clear)
{
	m_stack.push("top");
	m_stack.pop();
	check_stats(1, 0);
}

TEST_F(DeepStackTest, pop_first)
{
	m_stack.push("top");
	datatype value;
	m_stack.pop(value);
	EXPECT_STREQ("top", std::get<std::string>(value));
	check_stats(1, 0);
}

TEST_F(DeepStackTest, pop_last)
{
	m_stack.push(1, 2, 3, "top");
	datatype value;
	m_stack.pop(value);
	EXPECT_STREQ("top", std::get<std::string>(value));
	check_stats(1, 3);
}

TEST_F(DeepStackTest, pop_all)
{
	m_stack.push(1, 2, 3, "top");
	datatype v1, v2, v3, v4;
	m_stack.pop(v1, v2, v3, v4);
	EXPECT_STREQ("top", std::get<std::string>(v1));
	EXPECT_EQ(3, std::get<int>(v2));
	EXPECT_EQ(2, std::get<int>(v3));
	EXPECT_EQ(1, std::get<int>(v4));

	check_stats(1, 0);
}

TEST_F(DeepStackTest, next_buff)
{
	m_stack.push(1, 2, 3, 4, "top");
	EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));
	check_stats(2, 1);
}

TEST_F(DeepStackTest, pop_across_buffs)
{
	m_stack.push(1, 2, 3, 4, "top");
	EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));
	check_stats(2, 1);
	datatype v1, v2, v3, v4;
	m_stack.pop(v1, v2, v3, v4);
	EXPECT_STREQ("top", std::get<std::string>(v1));
	EXPECT_EQ(4, std::get<int>(v2));
	EXPECT_EQ(3, std::get<int>(v3));
	EXPECT_EQ(2, std::get<int>(v4));

	check_stats(1, 1);
}

TEST_F(DeepStackTest, realloc)
{
	for(int i = 1; i <= 16; ++i ) {
		m_stack.push(i);
	}
	m_stack.push("top");

	EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));
	check_stats(5, 1);
}

TEST_F(DeepStackTest, realloc_and_repush)
{
	m_stack.push("top");
	for(int i = 1; i <= 16; ++i ) {
		m_stack.push(i);
	}
	for(int i = 1; i <= 16; ++i ) {
		m_stack.pop();
	}
	EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));
	m_stack.pop();

	EXPECT_TRUE(m_stack.empty());
}

TEST_F(DeepStackTest, empty_and_repush)
{
	m_stack.push("top");
	for(int i = 1; i <= 16; ++i ) {
		m_stack.push(i);
	}
	for(int i = 1; i <= 16; ++i ) {
		m_stack.pop();
	}
	m_stack.pop();


	m_stack.push("bottom", 2, 3, 4, "top");
	EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));

	datatype v1, v2, v3, v4, v5;
	m_stack.pop(v1, v2, v3, v4, v5);
	EXPECT_STREQ("top", std::get<std::string>(v1));
	EXPECT_EQ(4, std::get<int>(v2));
	EXPECT_EQ(3, std::get<int>(v3));
	EXPECT_EQ(2, std::get<int>(v4));
	EXPECT_STREQ("bottom", std::get<std::string>(v5));

	EXPECT_TRUE(m_stack.empty());
}


TEST_F(DeepStackTest, shrink)
{
	m_stack.push("top");
	for(int i = 1; i <= 16; ++i ) {
		m_stack.push(i);
	}
	for(int i = 1; i <= 7; ++i ) {
		m_stack.pop();
	}

	m_stack.shrink_to_fit();
	size_t blocks, depth, curBlock, curData;
	m_stack.getStats(blocks, depth, curBlock, curData);
	EXPECT_EQ(3, blocks);
	EXPECT_EQ(2, curData);
	EXPECT_EQ(3, curBlock);
	EXPECT_EQ(10, m_stack.size());

	EXPECT_EQ(9, std::get<int>(m_stack.top()));
}

TEST_F(DeepStackTest, shrink_and_reuse)
{
	m_stack.push("top");
	for(int i = 1; i <= 16; ++i ) {
		m_stack.push(i);
	}
	for(int i = 1; i <= 7; ++i ) {
		m_stack.pop();
	}

	m_stack.shrink_to_fit();

	for(int i = 1; i <= 16; ++i ) {
		m_stack.push(i);
	}
	EXPECT_EQ(16, std::get<int>(m_stack.top()));

	for(int i = 1; i <= 16+9; ++i ) {
		m_stack.pop();
	}

	size_t blocks, depth, curBlock, curData;
	m_stack.getStats(blocks, depth, curBlock, curData);
	EXPECT_EQ(12, blocks);
	EXPECT_EQ(1, curData);
	EXPECT_EQ(1, curBlock);
	EXPECT_EQ(1, m_stack.size());

	EXPECT_EQ("top", std::get<std::string>(m_stack.top()));
}


TEST_F(DeepStackTest, direct_iterator)
{
	m_stack.push(1,2,3,4,5);
	check_iterator<DeepStack::iterator>(5, 1,
			[](int i){return i-1;}, [](int i){return i+1;},
			m_stack.begin(), m_stack.end());
}

TEST_F(DeepStackTest, direct_const_iterator)
{
	m_stack.push(1,2,3,4,5);
	check_iterator<DeepStack::const_iterator>(5, 1,
			[](int i){return i-1;}, [](int i){return i+1;},
			m_stack.cbegin(), m_stack.cend());
}

TEST_F(DeepStackTest, reverse_iterator)
{
	m_stack.push(1,2,3,4,5);
	check_iterator<DeepStack::reverse_iterator>(1, 5,
			[](int i){return i+1;},
			[](int i){return i-1;},
			m_stack.rbegin(), m_stack.rend());
}

TEST_F(DeepStackTest, reverse_const_iterator)
{
	m_stack.push(1,2,3,4,5);
	check_iterator<DeepStack::const_reverse_iterator>(1, 5,
			[](int i){return i+1;},
			[](int i){return i-1;},
			m_stack.crbegin(), m_stack.crend());
}

TEST_F(DeepStackTest, from_top)
{
	m_stack.push(1,2,3,4,5,6);
	check_iterator<DeepStack::reverse_iterator>(1, 6,
			[](int i){return i+1;},
			[](int i){return i-1;},
			m_stack.from_top(6), m_stack.rend());
}

TEST_F(DeepStackTest, peek)
{
	m_stack.push("bottom", 1, 2, 3, "top");

	datatype v1, v2, v3, v4, v5;
	m_stack.peek(v1, v2, v3, v4, v5);
	EXPECT_STREQ("top", std::get<std::string>(v1));
	EXPECT_EQ(3, std::get<int>(v2));
	EXPECT_EQ(2, std::get<int>(v3));
	EXPECT_EQ(1, std::get<int>(v4));
	EXPECT_STREQ("bottom", std::get<std::string>(v5));

	EXPECT_STREQ("top", std::get<std::string>( m_stack.top()));
}

TEST_F(DeepStackTest, peek_reverse_one)
{
	m_stack.push("bottom", 1, 2, 3, "top");

	datatype v1;
	m_stack.peek_depth(1, v1);
	EXPECT_STREQ("top", std::get<std::string>(v1));
	EXPECT_STREQ("top", std::get<std::string>( m_stack.top()));
}


TEST_F(DeepStackTest, peek_reverse)
{
	m_stack.push("bottom", 1, 2, 3, "top");

	datatype v1, v2, v3, v4, v5;
	m_stack.peek_reverse(v1, v2, v3, v4, v5);
	EXPECT_STREQ("top", std::get<std::string>(v5));
	EXPECT_EQ(3, std::get<int>(v4));
	EXPECT_EQ(2, std::get<int>(v3));
	EXPECT_EQ(1, std::get<int>(v2));
	EXPECT_STREQ("bottom", std::get<std::string>(v1));
}

TEST_F(DeepStackTest, peek_depth)
{
	m_stack.push("not read", "bottom", 1, 2, 3, "top");

	datatype v1, v2, v3, v4, v5;
	m_stack.peek_depth(5, v1, v2, v3, v4, v5);
	EXPECT_STREQ("top", std::get<std::string>(v5));
	EXPECT_EQ(3, std::get<int>(v4));
	EXPECT_EQ(2, std::get<int>(v3));
	EXPECT_EQ(1, std::get<int>(v2));
	EXPECT_STREQ("bottom", std::get<std::string>(v1));
}

TEST_F(DeepStackTest, discard_count)
{
	m_stack.push("new top", "bottom", 1, 2, 3, "top");

	m_stack.discard(0);
	EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));

	m_stack.discard(1);
	EXPECT_EQ(3, std::get<int>(m_stack.top()));
	m_stack.discard(1);
	EXPECT_EQ(2, std::get<int>(m_stack.top()));

	m_stack.discard(3);

	EXPECT_STREQ("new top", std::get<std::string>(m_stack.top()));
	m_stack.discard(1);
	EXPECT_TRUE(m_stack.empty());
	m_stack.discard(0);
	EXPECT_TRUE(m_stack.empty());
}


TEST_F(DeepStackTest, discard_direct)
{
	m_stack.push("new top", "bottom", 1, 2, 3, "top");

	auto iter = m_stack.begin();
	m_stack.discard(iter);
	EXPECT_EQ(3, std::get<int>(m_stack.top()));
	iter = m_stack.begin();
	m_stack.discard(iter);
	EXPECT_EQ(2, std::get<int>(m_stack.top()));

	iter =  m_stack.begin();
	++iter;
	m_stack.discard(iter);
	EXPECT_STREQ("bottom", std::get<std::string>(m_stack.top()));

	iter = m_stack.begin();
	m_stack.discard(iter);
	EXPECT_STREQ("new top", std::get<std::string>(m_stack.top()));

	iter = m_stack.begin();
	m_stack.discard(iter);
	EXPECT_TRUE(m_stack.empty());
}

TEST_F(DeepStackTest, discard_direct_all)
{
	m_stack.push("new top", "bottom", 1, 2, 3, "top");

	auto iter = m_stack.begin();
	iter += 4;
	m_stack.discard(iter);
	EXPECT_STREQ("new top", std::get<std::string>(m_stack.top()));
}

TEST_F(DeepStackTest, discard_reverse)
{
	m_stack.push("new top", "bottom", 1, 2, 3, "top");

	auto iter = m_stack.rbegin();
	iter += 4;
	m_stack.discard(iter);
	EXPECT_EQ(2, std::get<int>(m_stack.top()));
}

TEST_F(DeepStackTest, discard_reverse_all)
{
	m_stack.push("new top", "bottom", 1, 2, 3, "top");

	auto iter = m_stack.rbegin();
	m_stack.discard(iter);
	EXPECT_TRUE(m_stack.empty());
}


TEST_F(DeepStackTest, pop_reverse)
{
	m_stack.push("new top", "bottom", 1, 2, 3, "top");

	datatype v1, v2, v3, v4, v5;
	m_stack.pop_reverse(v1, v2, v3, v4, v5);
	EXPECT_STREQ("top", std::get<std::string>(v5));
	EXPECT_EQ(3, std::get<int>(v4));
	EXPECT_EQ(2, std::get<int>(v3));
	EXPECT_EQ(1, std::get<int>(v2));
	EXPECT_STREQ("bottom", std::get<std::string>(v1));

	EXPECT_STREQ("new top", std::get<std::string>(m_stack.top()));
}

FALCON_TEST_MAIN

/* end of deepstack.fut.cpp */
