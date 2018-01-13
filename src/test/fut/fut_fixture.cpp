/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: fut_fixture.cpp

  Test for fixtures
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 13 Jan 2018 21:06:37 +0000
  Touch : Sat, 13 Jan 2018 21:27:35 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>


class MyTest: public Falcon::test::TestCase
{
public:
   // Normally we don't need constructors,
   // we'll have one just to test that SetUp is REALLY called
   MyTest() {
      m_v = "";
   }

   void SetUp() {
      m_v = "SetUp";
   }

   void TearDown() {
      m_v = "TearDown";
   }

   const char* m_v;
};


TEST_F(MyTest, SetUpCalled) {
   EXPECT_SEQ("SetUp", m_v);

}

/* end of fut_fixture.cpp */

