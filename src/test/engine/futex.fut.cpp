/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: futex.fut.cpp

  Lightweight non-blocking mutex - unit test
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Fri, 12 Apr 2019 13:09:45 +0100
  Touch : Fri, 12 Apr 2019 13:09:45 +0100

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <falcon/futex.h>
#include <mutex>
#include <thread>
#include <vector>

using namespace Falcon;

class FutexTest: public Falcon::testing::TestCase
{
public:
   mutable Futex<0> m_futex;
   mutable Futex<1> m_yield_futex;
   mutable Futex<50> m_sl_futex;
   mutable std::mutex m_mutex;

   void SetUp() {
   }

   void TearDown() {
   }

   FutexTest(){}


   template<class _Mutex>
   void performance_test(_Mutex& mutex, int perfCount, int threadCount, int outOfBusyLoopCount)
   {
      volatile int counter = 0;
      auto check = [&](){
         int dummy = 1;
         for(int i = 0; i < perfCount; ++i) {
            // Simulate some out of the main loop operation
            for(int j = 0; j < outOfBusyLoopCount; ++j) {
               ++ dummy;
            }
            std::lock_guard guard(mutex);
            ++counter;
         }
      };


      std::vector<std::thread> threads;
      for (int i = 0; i < threadCount; ++i) {
         threads.emplace_back(check);
      }

      for (int i = 0; i < threadCount; ++i) {
         threads[i].join();
      }

      EXPECT_EQ(perfCount * threadCount, counter);
   }
};

TEST_F(FutexTest, smoke)
{
   EXPECT_FALSE(m_futex.isLocked());
   m_futex.lock();
   EXPECT_TRUE(m_futex.isLocked());
   m_futex.unlock();
   EXPECT_FALSE(m_futex.isLocked());
}

TEST_F(FutexTest, smoke2)
{
   EXPECT_FALSE(m_futex.isLocked());
   {
      std::lock_guard guard(m_futex);
      EXPECT_TRUE(m_futex.isLocked());
   }
   EXPECT_FALSE(m_futex.isLocked());
}


TEST_F(FutexTest, thread_counter)
{
   performance_test(m_futex, 10000, 10, 0);
}




TEST_F(FutexTest, perf_test_futex_trio)
{
   performance_test(m_futex, 10000000, 3, 30);
}

TEST_F(FutexTest, perf_test_yield_trio)
{
   performance_test(m_yield_futex, 10000000, 3, 30);
}

TEST_F(FutexTest, perf_test_sl_trio)
{
   performance_test(m_sl_futex, 10000000, 3, 30);
}

TEST_F(FutexTest, perf_test_mutex_trio)
{
   performance_test(m_mutex, 10000000, 3, 30);
}




TEST_F(FutexTest, perf_test_futex_contention)
{
   performance_test(m_futex, 10000000, 4, 0);
}

TEST_F(FutexTest, perf_test_yield_contention)
{
   performance_test(m_yield_futex, 10000000, 4, 0);
}

TEST_F(FutexTest, perf_test_sl_contention)
{
   performance_test(m_sl_futex, 10000000, 4, 0);
}

TEST_F(FutexTest, perf_test_mutex_contention)
{
   performance_test(m_mutex, 10000000, 4, 0);
}




TEST_F(FutexTest, perf_test_futex_non_contention)
{
   performance_test(m_futex, 1000000, 10, 1000);
}

TEST_F(FutexTest, perf_test_yield_non_contention)
{
   performance_test(m_yield_futex, 1000000, 10, 1000);
}

TEST_F(FutexTest, perf_test_sl_non_contention)
{
   performance_test(m_sl_futex, 1000000, 10, 1000);
}

TEST_F(FutexTest, perf_test_mutex_non_contention)
{
   performance_test(m_mutex, 1000000, 10, 1000);
}
FALCON_TEST_MAIN

/* end of futex.fut.cpp */
