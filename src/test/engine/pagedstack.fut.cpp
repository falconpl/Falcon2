/*****************************************************************************
FALCON2 - The Falcon Programming Language
FILE: pagedstack.fut.cpp

Test for PagedStack
-------------------------------------------------------------------
Author: Giancarlo Niccolai
Begin : Sat, 06 Apr 2019 16:17:19 +0100
Touch : Sat, 06 Apr 2019 16:17:19 +0100

-------------------------------------------------------------------
(C) Copyright 2019 The Falcon Programming Language
Released under Apache 2.0 License.
 ******************************************************************************/

#include <falcon/fut/fut.h>
#include <falcon/engine/pagedstack.h>
#include <variant>
#include <functional>
#include <sstream>

#include <iostream>

class PagedStackTest: public Falcon::testing::TestCase
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
   // Usinga mutex to check for deadlocks.
   using PagedStack = Falcon::PagedStack<datatype, std::allocator, std::mutex>;
   PagedStack m_stack{4,2};

   void SetUp() {}
   void TearDown() {}

   void check_stats(size_t blk, size_t cb, size_t cd) {
      size_t blocks, depth, curBlock, curData;
      m_stack.getStats(blocks, depth, curBlock, curData);

      EXPECT_EQ(blk, blocks);
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

   struct SharedMem {
      std::size_t m_allocCount{0};
      std::size_t m_allocSize{0};
      std::size_t m_deallocCount{0};
      std::size_t m_deallocSize{0};
      std::size_t m_totalAlloc{0};
   };

   template <typename _T>
   class TestAllocator
   {
   public:
      SharedMem* m_counters;
      SharedMem* m_large_counters;

   public:
      using value_type = _T;
      using size_type = std::size_t;
      using propagate_on_container_move_assignment = std::true_type;

      template <class U> struct rebind { typedef TestAllocator<U> other; };

      TestAllocator()noexcept  :m_counters{0} {}
      template <typename U>
      TestAllocator(const TestAllocator<U>& other) noexcept:m_counters{other.m_counters} {}

      TestAllocator(TestAllocator&& other) noexcept:
    	  m_counters(other.m_counters),
		  m_large_counters(other.m_large_counters)
	  {
         other.m_large_counters = other.m_counters = nullptr;
      }

      TestAllocator(const TestAllocator& other) noexcept:
    		  m_counters(other.m_counters),
			  m_large_counters(other.m_large_counters)
	  {}

      TestAllocator(SharedMem* ptr, SharedMem* lptr) noexcept:
    		  m_counters(ptr),
			  m_large_counters(lptr)
	  {}

      using pointer_type = _T*;
      using reference_type = _T&;

      pointer_type allocate(std::size_t size)
      {
    	  SharedMem* p = size > 2 ? m_large_counters : m_counters;
          if(p) {
            ++p->m_allocCount;
            p->m_allocSize += size;
            p->m_totalAlloc += size * sizeof(_T);}
          size_t allocSize = size * sizeof(_T);
          return static_cast<pointer_type>(operator new(allocSize));
      }

      void deallocate(void* data, std::size_t size) {
    	 SharedMem* p = size > 2 ? m_large_counters : m_counters;
         if(p) {
            ++p->m_deallocCount;
            p->m_deallocSize += size;
         }
         operator delete(data);
      }
   };
};


TEST_F(PagedStackTest, smoke)
{
   EXPECT_TRUE(m_stack.empty());
   EXPECT_EQ(0, m_stack.size());
}

TEST_F(PagedStackTest, simple_push)
{
   m_stack.push("Hello world");

   EXPECT_FALSE(m_stack.empty());
   EXPECT_EQ(1, m_stack.size());
   EXPECT_STREQ("Hello world", std::get<std::string>(m_stack.top()));
}

TEST_F(PagedStackTest, double_push)
{
   m_stack.push(1, "Hello world");

   EXPECT_FALSE(m_stack.empty());
   EXPECT_EQ(2, m_stack.size());
   EXPECT_STREQ("Hello world", std::get<std::string>(m_stack.top()));
}


TEST_F(PagedStackTest, simple_emplace)
{
   Falcon::PagedStack<SomeStruct> emplacer;

   emplacer.push_emplace(1, "Hello world");

   EXPECT_FALSE(emplacer.empty());
   EXPECT_EQ(1, emplacer.size());
   const SomeStruct& top = emplacer.top();

   EXPECT_EQ(1, top.alpha );
   EXPECT_STREQ("Hello world", top.beta);
}

TEST_F(PagedStackTest, double_emplace)
{
   Falcon::PagedStack<SomeStruct> emplacer;

   emplacer.push_emplace(0, "Hello world");
   emplacer.push_emplace(1, "Hello again");

   EXPECT_FALSE(emplacer.empty());
   EXPECT_EQ(2, emplacer.size());
   const SomeStruct& top = emplacer.top();

   EXPECT_EQ(1, top.alpha );
   EXPECT_STREQ("Hello again", top.beta);

}

TEST_F(PagedStackTest, fill_one_buffer)
{
   m_stack.push(1, 2, 3, "top");
   EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));
   check_stats(2, 1, 4);
}


TEST_F(PagedStackTest, pure_pop)
{
   m_stack.push("top");
   m_stack.push("discarded");
   m_stack.pop();
   EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));
   check_stats(2, 1, 1);
}

TEST_F(PagedStackTest, pure_pop_clear)
{
   m_stack.push("top");
   m_stack.pop();
   check_stats(2, 1, 0);
}

TEST_F(PagedStackTest, pop_first)
{
   m_stack.push("top");
   datatype value;
   m_stack.pop(value);
   EXPECT_STREQ("top", std::get<std::string>(value));
   check_stats(2, 1, 0);
}

TEST_F(PagedStackTest, pop_last)
{
   m_stack.push(1, 2, 3, "top");
   datatype value;
   m_stack.pop(value);
   EXPECT_STREQ("top", std::get<std::string>(value));
   check_stats(2, 1, 3);
}

TEST_F(PagedStackTest, pop_all)
{
   m_stack.push(1, 2, 3, "top");
   datatype v1, v2, v3, v4;
   m_stack.pop(v1, v2, v3, v4);
   EXPECT_STREQ("top", std::get<std::string>(v1));
   EXPECT_EQ(3, std::get<int>(v2));
   EXPECT_EQ(2, std::get<int>(v3));
   EXPECT_EQ(1, std::get<int>(v4));

   check_stats(2, 1, 0);
}

TEST_F(PagedStackTest, next_buff)
{
   m_stack.push(1, 2, 3, 4, "top");
   EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));
   check_stats(2, 2, 1);
}

TEST_F(PagedStackTest, pop_across_buffs)
{
   m_stack.push(1, 2, 3, 4, "top");
   EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));
   check_stats(2, 2, 1);
   datatype v1, v2, v3, v4;
   m_stack.pop(v1, v2, v3, v4);
   EXPECT_STREQ("top", std::get<std::string>(v1));
   EXPECT_EQ(4, std::get<int>(v2));
   EXPECT_EQ(3, std::get<int>(v3));
   EXPECT_EQ(2, std::get<int>(v4));

   check_stats(2, 1, 1);
}

TEST_F(PagedStackTest, realloc)
{
   for(int i = 1; i <= 16; ++i ) {
      m_stack.push(i);
   }
   m_stack.push("top");

   EXPECT_STREQ("top", std::get<std::string>(m_stack.top()));
   check_stats(6, 5, 1);
}

TEST_F(PagedStackTest, realloc_and_repush)
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

TEST_F(PagedStackTest, empty_and_repush)
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


TEST_F(PagedStackTest, shrink)
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

TEST_F(PagedStackTest, shrink_and_reuse)
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
   EXPECT_EQ(7, blocks);
   EXPECT_EQ(1, curData);
   EXPECT_EQ(1, curBlock);
   EXPECT_EQ(1, m_stack.size());

   EXPECT_EQ("top", std::get<std::string>(m_stack.top()));
}


TEST_F(PagedStackTest, direct_iterator)
{
   m_stack.push(1,2,3,4,5);
   check_iterator<PagedStack::iterator>(5, 1,
         [](int i){return i-1;}, [](int i){return i+1;},
         m_stack.begin(), m_stack.end());
}

TEST_F(PagedStackTest, direct_const_iterator)
{
   m_stack.push(1,2,3,4,5);
   check_iterator<PagedStack::const_iterator>(5, 1,
         [](int i){return i-1;}, [](int i){return i+1;},
         m_stack.cbegin(), m_stack.cend());
}

TEST_F(PagedStackTest, reverse_iterator)
{
   m_stack.push(1,2,3,4,5);
   check_iterator<PagedStack::reverse_iterator>(1, 5,
         [](int i){return i+1;},
         [](int i){return i-1;},
         m_stack.rbegin(), m_stack.rend());
}

TEST_F(PagedStackTest, reverse_const_iterator)
{
   m_stack.push(1,2,3,4,5);
   check_iterator<PagedStack::const_reverse_iterator>(1, 5,
         [](int i){return i+1;},
         [](int i){return i-1;},
         m_stack.crbegin(), m_stack.crend());
}

TEST_F(PagedStackTest, sync_iterator)
{
   m_stack.push(1,2,3,4,5);
   int i = 5;
   for(auto iter = m_stack.sync_begin(); iter != m_stack.sync_end(); ++iter) {
      datatype j;
      iter.get(j);
      EXPECT_EQ(i--, std::get<int>(j));
   }

   EXPECT_EQ(0, i);
}

TEST_F(PagedStackTest, sync_iterator_shrink)
{
   size_t blocks, depth, curBlock, curData;
   // this test checks if the reverse iterator resists stack modification.
   m_stack.push(1, 2, 3, 4, 5, "one", "two", "three");
   {
      auto si = m_stack.sync_begin();
      datatype one, two, three;
      m_stack.pop(three, two, one);
      EXPECT_EQ("three", std::get<std::string>(three));
      EXPECT_EQ("two", std::get<std::string>(two));
      EXPECT_EQ("one", std::get<std::string>(one));

      datatype svalue;
      si.get(svalue); EXPECT_EQ(5, std::get<int>(svalue)); ++si;
      si.get(svalue); EXPECT_EQ(4, std::get<int>(svalue)); ++si;
      si.get(svalue); EXPECT_EQ(3, std::get<int>(svalue)); ++si;

      datatype five, four;
      m_stack.pop(five, four);
      EXPECT_EQ(5, std::get<int>(five));
      EXPECT_EQ(4, std::get<int>(four));

      // The shrink-to-fit request will be fulfilled later.
      m_stack.shrink_to_fit();
      m_stack.getStats(blocks, depth, curBlock, curData);
      EXPECT_EQ(blocks, 2);

      int i = 2;
      for(; si != m_stack.sync_end(); ++si) {
         datatype j;
         si.get(j);
         EXPECT_EQ(i--, std::get<int>(j));
      }
      EXPECT_EQ(0, i);
   }

   m_stack.getStats(blocks, depth, curBlock, curData);
   EXPECT_EQ(1, blocks);
}


TEST_F(PagedStackTest, sync_iterator_jump_base)
{
   // this test checks if the reverse iterator resists stack modification.
   for(int p = 0; p <= 16; ++p) {
      m_stack.push(p);
   }

   auto si = m_stack.sync_begin();
   int i = 16;
   datatype svalue;

   // jump two blocks.
   for(; i >= 8; --i) {
      si.get(svalue); EXPECT_EQ(i, std::get<int>(svalue)); ++si;
   }

   // discard one block
   m_stack.discard(5);
   // but we should still be in synch
   si.get(svalue); EXPECT_EQ(7, std::get<int>(svalue)); ++si;

   //Now discard the current block
   m_stack.discard(8);
   // we expect to jump and have the new top
   si.get(svalue); EXPECT_EQ(3, std::get<int>(svalue)); ++si;
   si.get(svalue); EXPECT_EQ(3, std::get<int>(m_stack.top())); ++si;
}


TEST_F(PagedStackTest, from_top)
{
   m_stack.push(1,2,3,4,5,6);
   check_iterator<PagedStack::reverse_iterator>(1, 6,
         [](int i){return i+1;},
         [](int i){return i-1;},
         m_stack.from_top(6), m_stack.rend());
}

TEST_F(PagedStackTest, peek)
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

TEST_F(PagedStackTest, peek_reverse_one)
{
   m_stack.push("bottom", 1, 2, 3, "top");

   datatype v1;
   m_stack.peek_depth(1, v1);
   EXPECT_STREQ("top", std::get<std::string>(v1));
   EXPECT_STREQ("top", std::get<std::string>( m_stack.top()));
}


TEST_F(PagedStackTest, peek_reverse)
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

TEST_F(PagedStackTest, peek_depth)
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

TEST_F(PagedStackTest, discard_count)
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


TEST_F(PagedStackTest, discard_direct)
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

TEST_F(PagedStackTest, discard_direct_all)
{
   m_stack.push("new top", "bottom", 1, 2, 3, "top");

   auto iter = m_stack.begin();
   iter += 4;
   m_stack.discard(iter);
   EXPECT_STREQ("new top", std::get<std::string>(m_stack.top()));
}

TEST_F(PagedStackTest, discard_reverse)
{
   m_stack.push("new top", "bottom", 1, 2, 3, "top");

   auto iter = m_stack.rbegin();
   iter += 4;
   m_stack.discard(iter);
   EXPECT_EQ(2, std::get<int>(m_stack.top()));
}

TEST_F(PagedStackTest, discard_reverse_all)
{
   m_stack.push("new top", "bottom", 1, 2, 3, "top");

   auto iter = m_stack.rbegin();
   m_stack.discard(iter);
   EXPECT_TRUE(m_stack.empty());
}


TEST_F(PagedStackTest, pop_reverse)
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

TEST_F(PagedStackTest, allocator_smoke)
{
   SharedMem dataAlloc;
   SharedMem pageAlloc;

   using dstack = Falcon::PagedStack<int, TestAllocator>;
   {
      auto testalloc = dstack(4,2,
            dstack::allocator_type(&pageAlloc, &dataAlloc));

      EXPECT_EQ(2, dataAlloc.m_allocCount);
      EXPECT_EQ(8, dataAlloc.m_allocSize);
      EXPECT_EQ(2, pageAlloc.m_allocCount);
      EXPECT_EQ(2, pageAlloc.m_allocSize);
   }

   EXPECT_EQ(dataAlloc.m_deallocCount, dataAlloc.m_allocCount);
   EXPECT_EQ(dataAlloc.m_deallocSize, dataAlloc.m_allocSize);
   EXPECT_EQ(pageAlloc.m_deallocCount, pageAlloc.m_allocCount);
   EXPECT_EQ(pageAlloc.m_deallocSize, pageAlloc.m_allocSize);
}

TEST_F(PagedStackTest, allocator)
{
   SharedMem dataAlloc;
   SharedMem pageAlloc;

   using dstack = Falcon::PagedStack<std::string, TestAllocator>;
   {
      auto testalloc = dstack(4,2, TestAllocator<dstack::value_type>(&pageAlloc, &dataAlloc));

      testalloc.push(
            "one", "two", "three", "four", /* page 1 */
            "five", "six", "seven", "eight", /* page 2 */
            "nine", "ten", "eleven", "twelve" /* page 3 */
      );

      EXPECT_EQ(4, dataAlloc.m_allocCount);
      EXPECT_EQ(16, dataAlloc.m_allocSize);
      EXPECT_EQ(4, pageAlloc.m_allocCount);
      EXPECT_EQ(4, pageAlloc.m_allocSize);

      std::string twelve, eleven, ten, nine, eight;
      testalloc.pop(twelve, eleven, ten, nine, eight);
      EXPECT_STREQ("twelve", twelve);
      EXPECT_STREQ("eleven", eleven);
      EXPECT_STREQ("ten", ten);
      EXPECT_STREQ("nine", nine);
      EXPECT_STREQ("eight", eight);

      // we expect no further allocation to take place...
      testalloc.push(twelve, eleven, ten, nine, eight, "thirtheen");

      EXPECT_EQ(4, dataAlloc.m_allocCount);
      EXPECT_EQ(16, dataAlloc.m_allocSize);
      EXPECT_EQ(4, pageAlloc.m_allocCount);
      EXPECT_EQ(4, pageAlloc.m_allocSize);
   }

   EXPECT_EQ(dataAlloc.m_deallocCount, dataAlloc.m_allocCount);
   EXPECT_EQ(dataAlloc.m_deallocSize, dataAlloc.m_allocSize);
   EXPECT_EQ(pageAlloc.m_deallocCount, pageAlloc.m_allocCount);
   EXPECT_EQ(pageAlloc.m_deallocSize, pageAlloc.m_allocSize);
}

FALCON_TEST_MAIN

/* end of pagedstack.fut.cpp */
