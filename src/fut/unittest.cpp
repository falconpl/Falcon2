/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: unittest.cpp

  Main unit test engine
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 16:38:29 +0000
  Touch : Tue, 09 Jan 2018 23:27:02 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/unittest.h>
#include <falcon/fut/testcase.h>

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <iostream>
#include <sstream>

namespace Falcon {
namespace test{

class UnitTest::Private {
public:
   Private():
      status(0),
      screenWidth(50),
      opt_erroutIsFail(false),
      verbosity(UnitTest::REPORT_STATUS)
   {}

   std::vector<TestCase* > tests;
   std::map< std::string, TestCase*> testsByName;
   int status;
   int screenWidth;

   bool opt_erroutIsFail;
   UnitTest::t_verbosity verbosity;

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
   p->testsByName.insert(std::make_pair(tcase->name(), tcase));
}

void UnitTest::setup()
{
   p->status = 0;
   for(auto testIter: p->tests ) {
      testIter->setup();
   }
}

void UnitTest::runAllTests()
{
   int count = 0;
   for(auto tcase: p->tests ) {
      beginTest(++count, tcase);
      tcase->run();
      endTest(count, tcase);

      if (!hasPassed(tcase))
      {
         p->status = -1;
      }
   }
}

void UnitTest::beginTest(int count, TestCase* tc)
{
   if(p->verbosity >= REPORT_BEGIN) {
      writeTestName(count, tc->name(), "GO");
   }

   tc->beginTest();
}

void UnitTest::endTest(int count, TestCase* tc)
{
   tc->endTest();

   if(tc->capturedOut()[0]
      && (p->verbosity >= REPORT_STDOUT || (p->verbosity > SILENT && !hasPassed(tc)))) {
      std::cout << "\n======================================================================\n";
      std::cout << "STDOUT " << tc->name() << "\n";
      std::cout << "======================================================================\n";
      std::cout << tc->capturedOut() << "\n";
   }

   if(tc->capturedErr()[0]
      && (p->verbosity >= REPORT_STDERR || (p->verbosity > SILENT && !hasPassed(tc)))) {
      std::cout << "\n======================================================================\n";
      std::cout << "STDERR " << tc->name() << "\n";
      std::cout << "======================================================================\n";
      std::cout << tc->capturedErr() << "\n";
   }

   if(p->verbosity > SILENT && tc->status() == TestCase::ERROR) {
      std::cout << "\n======================================================================\n";
      std::cout << "Uncaught error in Case " << count << ": " << tc->name() << "\n";
      std::cout << tc->failDesc() << "\n";
   }
   else if(p->verbosity >= REPORT_FAILURE && tc->status() == TestCase::FAIL)
   {
      std::cout << "\n======================================================================\n";
      std::cout << "Failure in case " << count << ": " << tc->name() << " at "
                << tc->failFile() << ": " << tc->failLine() << "\n";
      std::cout << tc->failDesc() << "\n";
   }
   else if(p->verbosity >= REPORT_STATUS) {
      writeTestName(count, tc->name(), hasPassed(tc) ? "OK" : "FAIL");
   }
}

void UnitTest::writeTestName(int count, const char* tname, const char* result)
{
   std::ostringstream sname;
   sname << "[Case " << count << ": " << tname;
   int tnamelen = p->screenWidth - sname.str().length();
   while(tnamelen > 0) {
      sname << " ";
      --tnamelen;
   }

   std::string sres = result;
   while(sres.length() < 4) sres += ' ';
   sres += ']';
   std::cout << sname.str().substr(0, p->screenWidth) << sres << '\n';
}

bool UnitTest::hasPassed(TestCase* tcase) const
{
   return ! ((tcase->status() == TestCase::OUT_ON_ERROR_STREAM && p->opt_erroutIsFail)
            || tcase->status() != TestCase::SUCCESS);
}

void UnitTest::report()
{
   if(p->verbosity > SILENT) {
      std::cout << "\n======================================================================\n";
      if (p->status != 0) {
         bool first = true;
         for(auto tcase: p->tests ) {
            if (!hasPassed(tcase))
            {
               std::cout << (first ? "Failed cases: " : ", ") << tcase->name();
               first = false;
            }
         }
         std::cout << "\n======================================================================\n";
      }
      std::cout << "Unit Test " <<( p->status == 0 ? "Passed\n" : "FAILED \n");
   }
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


int UnitTest::performUnitTests()
{
   setup();
   runAllTests();
   report();
   return p->status;
}


int UnitTest::performTest(const char* name)
{
   setup();

   auto titer = p->testsByName.find(name);
   if(titer == p->testsByName.end()) {
      std::ostringstream ss;
      ss << "Test '" << name << "' not found";
      throw std::runtime_error(ss.str());
   }


   beginTest(1, titer->second);
   titer->second->run();
   endTest(1, titer->second);

   if (!hasPassed(titer->second))
   {
     p->status = -1;
     return -1;
   }

   return 0;
}


void UnitTest::setVerbosity( t_verbosity vb )
{
   p->verbosity = vb;
}


void UnitTest::parseParams(int argc, char* argv[])
{
}


int UnitTest::main(int argc, char* argv[])
{
   parseParams(argc, argv);
   int status = performUnitTests();

   tearDown();
   delete this;
   return status;
}

}
}

/* end of unittest.cpp */
