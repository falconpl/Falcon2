/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: gc.fut.cpp

  Generic Garbage Collector
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 13 Apr 2019 10:05:28 +0100
  Touch : Sat, 13 Apr 2019 10:05:28 +0100

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <falcon/engine/gc.h>


using namespace Falcon;

class GCTest: public Falcon::testing::TestCase
{
public:
   GarbageCollector m_gc;

   class Handler: public GarbageCollector::Handler {
   public:
      int m_destructed{0};
      int m_marked{0};

      // The vector will not actually be managed by the gc,
      // it is just a structure we will use as a reachable root of data.
      std::vector<void *>* m_allocated;

      virtual void destroy(void* target) noexcept override
      {
         m_destructed ++;
      }

      virtual void mark(void* target, GarbageCollector* owner) noexcept override
      {
         for( auto entry: *m_allocated ) {
            owner->mark(entry);
         }
         m_marked ++;
      }
   };

   Handler m_handler;


   void SetUp() {
      void* memory = m_gc.getMemory(sizeof(std::vector<void*>), &m_handler);
      m_handler.m_allocated = new(memory) std::vector<void*>;
   }

   void TearDown() {}

   GCTest(){}
};


TEST_F(GCTest, smoke)
{
   GarbageCollector gc;
   GarbageCollector::Stats stats = gc.getStats();
   EXPECT_EQ(0, stats.all_allocated);
   EXPECT_EQ(0, stats.all_blocks);
}


TEST_F(GCTest, smoke2)
{
   GarbageCollector gc;
   size_t* allocated = gc.getDataBuffer<size_t>(2);
   GarbageCollector::Stats stats = gc.getStats();
   EXPECT_EQ(sizeof(size_t)*2, stats.all_allocated);
   EXPECT_EQ(1, stats.all_blocks);
   EXPECT_EQ(0, stats.white_allocated);
   EXPECT_EQ(0, stats.white_blocks);
   EXPECT_EQ(sizeof(size_t) * 2, stats.gray_allocated);
   EXPECT_EQ(1, stats.gray_blocks);
   EXPECT_EQ(0, stats.black_allocated);
   EXPECT_EQ(0, stats.black_blocks);
}

TEST_F(GCTest, smoke3)
{
   for(int i = 0; i < 10; ++i){
      m_handler.m_allocated->push_back(m_gc.getDataBuffer<int>(1));
      *static_cast<int*>(m_handler.m_allocated->back()) = i;
      m_gc.release(m_handler.m_allocated->back());
   }

   GarbageCollector::Stats stats = m_gc.getStats();
   EXPECT_EQ(sizeof(int)*10 + sizeof(std::vector<int>), stats.all_allocated);
   EXPECT_EQ(11, stats.all_blocks);
   EXPECT_EQ(0, stats.white_allocated);
   EXPECT_EQ(0, stats.white_blocks);
   EXPECT_EQ(sizeof(int)*10 + sizeof(std::vector<int>), stats.gray_allocated);
   EXPECT_EQ(11, stats.gray_blocks);
   EXPECT_EQ(0, stats.black_allocated);
   EXPECT_EQ(0, stats.black_blocks);

}


/* end of gc.fut.cpp */
