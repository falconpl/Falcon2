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

using namespace Falcon;

class FutexTest: public Falcon::testing::TestCase
{
public:
   mutable Futex m_futex;

   void SetUp() {
   }

   void TearDown() {
   }

   FutexTest(){}
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
      std::lock_guard<Futex> guard(m_futex);
      EXPECT_TRUE(m_futex.isLocked());
   }
   EXPECT_FALSE(m_futex.isLocked());
}

FALCON_TEST_MAIN

/* end of futex.fut.cpp */
