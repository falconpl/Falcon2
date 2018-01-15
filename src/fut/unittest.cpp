/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: unittest.cpp

  Main unit test engine
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 16:38:29 +0000
  Touch : Mon, 15 Jan 2018 00:20:14 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/unittest.h>
#include <falcon/fut/testcase.h>
#include <falcon/fut/timelapse.h>

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

#include <iostream>
#include <fstream>
#include <sstream>

namespace Falcon {
namespace test{

namespace {
class Component {
public:
   Component(const std::string& n):
      name(n),
      failures(0)
   {}

   int failures;
   std::string name;
   std::vector<TestCase*> tests;
   TimeLapse lapse;
};
}

class UnitTest::Private {
public:
   Private():
      status(0),
      screenWidth(50),
      opt_erroutIsFail(false),
      verbosity(UnitTest::REPORT_STATUS)
   {}

   std::vector<TestCase* > tests;
   std::map<std::string, TestCase*> testsByName;
   std::multimap<std::string, TestCase*> testsByComponent;
   std::map<std::string, Component> componentToPerform;
   std::vector<TestCase*> testsToPerofm;
   std::string xmlReport;

   int status;
   int screenWidth;
   int failedCount;

   bool opt_erroutIsFail;
   UnitTest::t_verbosity verbosity;
   TimeLapse lapse;
};


UnitTest::UnitTest():
   p(new Private)
{
}


UnitTest::~UnitTest()
{
   delete p;
}

TestCase* UnitTest::addTestCase(const char* component, const char* caseName, TestCase* tcase)
{
   tcase->setName(component, caseName);
   p->tests.push_back(tcase);
   p->testsByName.insert(std::make_pair(tcase->fullName(), tcase));
   p->testsByComponent.insert(std::make_pair(tcase->componentName(), tcase));

   return tcase;
}

void UnitTest::init()
{
   p->status = 0;
   p->failedCount = 0;
   for(auto testIter: p->tests ) {
      testIter->init();
   }
}

void UnitTest::runAllTests()
{
   // Reorder the tests in components
   int count = 0;
   for(auto tcase: p->testsToPerofm ) {

      auto catIter = p->componentToPerform.find(tcase->componentName());
      Component* component;
      if(catIter == p->componentToPerform.end()) {
         component = &p->componentToPerform.insert(
            std::make_pair(tcase->componentName(),
               Component(tcase->componentName()))).first->second;
      }
      else {
         component = &catIter->second;
      }
      component->tests.push_back(tcase);
   }

   for(auto& itcat: p->componentToPerform) {
      auto& component = itcat.second;
      component.lapse.markBegin();

      for(auto tcase: component.tests ) {
         beginTest(++count, tcase);
         tcase->run();
         endTest(count, tcase);

         if (!hasPassed(tcase))
         {
            p->status = -1;
            p->failedCount++;
            component.failures++;
         }
      }
      component.lapse.markEnd();
   }
}

void UnitTest::beginTest(int count, TestCase* tc)
{
   if(p->verbosity >= REPORT_BEGIN) {
      writeTestName(count, tc->fullName(), "GO");
   }

   tc->beginTest();
}

void UnitTest::endTest(int count, TestCase* tc)
{
   tc->endTest();

   if(tc->capturedOut()[0]
      && (p->verbosity >= REPORT_STDOUT || (p->verbosity > SILENT && !hasPassed(tc)))) {
      std::cout << "\n======================================================================\n";
      std::cout << "STDOUT " << tc->fullName() << "\n";
      std::cout << "======================================================================\n";
      std::cout << tc->capturedOut() << "\n";
   }

   if(tc->capturedErr()[0]
      && (p->verbosity >= REPORT_STDERR || (p->verbosity > SILENT && !hasPassed(tc)))) {
      std::cout << "\n======================================================================\n";
      std::cout << "STDERR " << tc->fullName() << "\n";
      std::cout << "======================================================================\n";
      std::cout << tc->capturedErr() << "\n";
   }

   if(p->verbosity > SILENT && tc->status() == TestCase::ERROR) {
      std::cout << "\n======================================================================\n";
      std::cout << "Uncaught error in Case " << count << ": " << tc->fullName() << "\n";
      std::cout << tc->failDesc() << "\n";
   }
   else if(p->verbosity >= REPORT_FAILURE && tc->status() == TestCase::FAIL)
   {
      std::cout << "\n======================================================================\n";
      std::cout << "Failure in case " << count << ": " << tc->fullName() << " at "
                << tc->failFile() << ": " << tc->failLine() << "\n";
      std::cout << tc->failDesc() << "\n";
   }
   else if(p->verbosity >= REPORT_STATUS) {
      writeTestName(count, tc->fullName(), hasPassed(tc) ? "OK" : "FAIL");
   }
}

void UnitTest::writeTestName(int count, const char* tname, const char* result)
{
   std::ostringstream sname;
   sname << "[" << count << ": " << tname;
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
      if (p->status != 0) {
         bool first = true;
         for(auto tcase: p->tests ) {
            if (!hasPassed(tcase))
            {
               if(first) {
                  std::cout << "\n======================================================================"
                     << "\nFailed cases: ";
                  first = false;
               }
               else {
                  std::cout << ", ";
               }
               std::cout << tcase->fullName();
            }
         }
         if(!first) {
            std::cout << "\n======================================================================\n";
         }
      }
      std::cout << "Unit Test " <<( p->status == 0 ? "Passed\n" : "FAILED \n");
   }
}


void UnitTest::destroy()
{
   for(auto testIter: p->tests ) {
      testIter->destroy();
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
   init();
   // fill the test to perform with all the tests, if none was explicitly given.
   auto& testsToPerform = p->testsToPerofm;
   if(testsToPerform.empty()) {
      testsToPerform = p->tests;
   }

   p->lapse.begin();
   runAllTests();
   p->lapse.end();

   report();
   if(!p->xmlReport.empty()) {
      saveToXML();
   }
   return p->status;
}


int UnitTest::performTest(const char* name)
{
   init();

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


int UnitTest::parseParams(int argc, char* argv[])
{
   for(int i = 0; i < argc; ++i)
   {
      std::string opt = argv[i];

      if(opt == "-q") {
         setVerbosity(SILENT);
         continue;
      }
      else if(opt == "-v" && ++i < argc) {
         std::istringstream is(argv[i]);
         int level;
         is >> level;
         if(level >= SILENT && level <= REPORT_STDOUT) {
            setVerbosity(static_cast<t_verbosity>(level));
            continue;
         }
      }
      else if (opt == "-t" && ++i < argc) {
         std::string testName = argv[i];
         auto iter = p->testsByName.find(testName);
         if(iter == p->testsByName.end()) {
            std::cout << "Unknown test case " << testName << "\n";
            return 1;
         }

         p->testsToPerofm.push_back(iter->second);
         continue;
      }
      else if (opt == "-o" && ++i < argc) {
         p->xmlReport = argv[i];
         continue;
      }
      else if(opt == "-h") {
         usage();
         return 1;
      }

      // If we're here, we have invalid options
      std::cerr << "Invalid options\n";
      usage();
      return 1;
   }

   return 0;
}


void UnitTest::usage() {
   std::cout << "UnitTest command line options:\n\n"
      << "\t-h\tThis help\n"
      << "\t-q\tRun quietly (suppress all output)\n"
      << "\t-o FILE\tSave XML report to this file\n"
      << "\t-t NAME\tRun given test (may be used multiple times)\n"
      << "\t-v N\tSets verbosity level to N (" << SILENT << "=silent, "
      << REPORT_STDOUT <<"=all)\n\n";
}


void UnitTest::saveToXML()
{
   std::ofstream fout(p->xmlReport);
   if(! fout) {
      std::cerr << "Cannot save XML report to " << p->xmlReport;
      return;
   }

   fout << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
   fout << "<testsuites id=\"\" name=\"UnitTests\" tests=\""
            << p->testsToPerofm.size() << "\" "
            << "failures=\"" << p->failedCount << "\" "
            << "time=\""<< p->lapse.elapsed() / 1000.0 << "\">\n";
   for(auto& icat: p->componentToPerform) {
      auto& component = icat.second;

      fout << "<testsuite id=\"\" "
           << "name=\"" << component.name << "\" "
           << "tests=\"" << component.tests.size() << "\" "
           << "failures=\"" << component.failures << "\" "
           << "time=\"" << component.lapse.elapsed() / 1000.0 << "\">\n";

      for(auto tcasep: component.tests) {
         fout << "<testcase id=\"\" "
              << "name=\"" << tcasep->caseName() << "\" "
              << "time=\"" << tcasep->elapsedTime() / 1000.0 << "\">\n";

         if(!hasPassed(tcasep)) {
            fout << "<failure message=\"" << tcasep->failDesc() << "\" "
                 << "type=\"" << tcasep->statusDesc() << "\">\n";
            fout << tcasep->failDesc() << "\n"
                 << "File: " << tcasep->failFile() << "\n"
                 << "Line: " << tcasep->failLine() << "\n";
            fout << "</failure>\n";
         }
         fout << "</testcase>\n";
      }
      fout << "</testsuite>\n";
   }

   fout << "</testsuites>\n";
}


int UnitTest::main(int argc, char* argv[])
{
   int status = parseParams(argc, argv);
   if(!status) {
      status = performUnitTests();
      destroy();
   }

   delete this;
   return status;
}

}
}

/* end of unittest.cpp */
