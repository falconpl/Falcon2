/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: fut_fixture.cpp

  Test for fixtures
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 13 Jan 2018 21:06:37 +0000
  Touch : Tue, 16 Jan 2018 22:29:26 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <iostream>

class MyTest: public Falcon::testing::TestCase
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
   EXPECT_STREQ("SetUp", m_v);
}

int main(int argc, char* argv[])
{
   Falcon::testing::UnitTest* ut = Falcon::testing::UnitTest::singleton();
   ut->detectTestName(argv[0]);
   if (!ut->parseParams(argc - 1, argv + 1)) {
      return 0;
   }
   if (0 != ut->performTest("MyTest::SetUpCalled")) {
      return 1;
   }

   if( std::string(FALCON_TEST_CLASS_NAME(MyTest,SetUpCalled)::_instance_->m_v) != "TearDown" ) {
      std::cerr<< "TearDown failed";
      return 1;
   }

   return 0;
}


/* end of fut_fixture.cpp */

