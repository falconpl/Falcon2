/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: fut.cpp

  Unit test sub-system
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 13:55:57 +0000
  Touch : Tue, 09 Jan 2018 15:30:48 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>

#include <set>
#include <string>
#include <vector>

#include <iostream>

namespace Falcon {
namespace test {

class UnitTest::Private {
public:
   Private():
      status(0)
   {}

   std::vector<TestCase* > tests;
   std::set< std::pair< std::string, TestCase*> > testsByName;
   int status;
};

UnitTest::UnitTest():
   p(new Private)
{
}

UnitTest::~UnitTest()
{
   delete p;
}

void UnitTest::addTestCase(TestCase* tcase)
{
   p->tests.push_back(tcase);
   //p->testsByName.insert(std::make_pair<tcase->m_name, tcase>);
}

void UnitTest::setup()
{
   //TODO
   for(auto testIter: p->tests ) {
      testIter->setup();
   }
}

void UnitTest::runAllTests()
{
   for(auto testIter: p->tests ) {
      testIter->beginTest();
      testIter->run();
      testIter->endTest();
      if (! testIter->isSuccess()) {
         p->status = 1;
      }
   }
}

void UnitTest::report()
{
   // TODO
}

void UnitTest::tearDown()
{
   for(auto testIter: p->tests ) {
      testIter->teardown();
   }
   p->tests.clear();
   p->testsByName.clear();
}

UnitTest* UnitTest::singleton(){
   static UnitTest* theTest = 0;
   if (theTest == 0) {
      theTest = new UnitTest;
   }

   return theTest;
}



int UnitTest::main(int argc, char* argv[])
{
   setup();
   runAllTests();
   int status = p->status;

   tearDown();
   delete this;

   return status;
}



TestCase::TestCase(const char* name):
   m_name(name),
   m_success(false),
   m_complete(false)
{
   UnitTest::singleton()->addTestCase(this);
}

TestCase::~TestCase()
{
}

void TestCase::setup()
{
   //TODO
}

void TestCase::teardown()
{
   //TODO
}

void TestCase::beginTest()
{
   //TODO
}

void TestCase::endTest()
{
   //TODO
}

}
}


/* end of fut.cpp */

