/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: fut_verbosity.cpp

  Test for correct output at verbosity levels
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 22:12:28 +0000
  Touch : Sat, 13 Jan 2018 21:50:53 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

#define EXPLICIT_FAILURE "Explicit failure!"
#define EXPLICIT_THROW "Explicit throw!"
#define EXPLICIT_ERROR "An error written on cerr\n"
#define SENTENCE_STDOUT "A sentence on COUT\n"

FALCON_TEST(Verbosity, onCerr) {
   std::cout << SENTENCE_STDOUT;
   std::cerr << EXPLICIT_ERROR;
}

FALCON_TEST(Verbosity, TestFail) {
   std::cout << SENTENCE_STDOUT;
   EXPECT_EQ(10,20);
}

FALCON_TEST(Verbosity, ExplicitFail) {
   FAIL(EXPLICIT_FAILURE);
}

FALCON_TEST(Verbosity, ExplicitThrow) {
   throw std::runtime_error(EXPLICIT_THROW);
}

namespace {

void doTest(const std::string& name, std::string& out, std::string& err)
{
   Falcon::test::UnitTest* ut = Falcon::test::UnitTest::singleton();
   std::ostringstream outCapture;
   std::ostringstream errCapture;

   std::streambuf *outbuf = std::cout.rdbuf();
   std::streambuf *errbuf = std::cerr.rdbuf();

   std::cout.rdbuf(outCapture.rdbuf());
   std::cerr.rdbuf(errCapture.rdbuf());

   try{
      int result = ut->performTest(name.c_str());
      if(!result) {
         std::ostringstream ss;
         ss << "Test " << name << " was meant to fail, but it succeeded";
         throw std::runtime_error(ss.str());
      }
      out = outCapture.str();
      err = errCapture.str();
      std::cout.rdbuf(outbuf);
      std::cerr.rdbuf(errbuf);

   }
   catch(...) {
      std::cout.rdbuf(outbuf);
      std::cerr.rdbuf(errbuf);
      throw;
   }
}

int test( int id )
{
   Falcon::test::UnitTest* ut = Falcon::test::UnitTest::singleton();
   std::string out, err;

   switch(id) {
   case 1:
      ut->setVerbosity(Falcon::test::UnitTest::SILENT);
      // The test must fail but nothing should be printed
      doTest("Verbosity::ExplicitFail", out, err);
      if(!out.empty() || !err.empty()) {
         return 1;
      }
      break;

   case 2:
         ut->setVerbosity(Falcon::test::UnitTest::REPORT_FAILURE);
         doTest("Verbosity::ExplicitFail", out, err);
         if(out.find(EXPLICIT_FAILURE) == std::string::npos || !err.empty()) {
            return 1;
         }
         break;

   case 3:
         ut->setVerbosity(Falcon::test::UnitTest::REPORT_STATUS);
         doTest("Verbosity::TestFail", out, err);
         if(out.find(SENTENCE_STDOUT) == std::string::npos || !err.empty()) {
            return 1;
         }
         break;

   case 4:
         ut->setVerbosity(Falcon::test::UnitTest::REPORT_STDERR);
         doTest("Verbosity::onCerr", out, err);
         if(out.find(SENTENCE_STDOUT) == std::string::npos ||
            out.find(EXPLICIT_ERROR) == std::string::npos) {
            return 1;
         }
         break;

   case 5:
           ut->setVerbosity(Falcon::test::UnitTest::REPORT_STDOUT);
           doTest("Verbosity::onCerr", out, err);
           if(out.find(SENTENCE_STDOUT) == std::string::npos ||
              out.find(EXPLICIT_ERROR) == std::string::npos) {
              return 1;
           }
           break;

   default:
      return -1;
   }

   return 0;
}

}

int main(int argc, char* argv[])
{
   Falcon::test::UnitTest* ut = Falcon::test::UnitTest::singleton();
   ut->parseParams(argc, argv);

   int result = 0;
   int testId = 0;
   while(result==0) {
      try {
         result = test(++testId);
      }
      catch(std::runtime_error& err) {
         std::cerr << "Test n. " << testId
            << " caused an exception: " << err.what() << "\n";
         return 1;
      }
   }

   if (result > 0) {
      std::cerr << "Test n. " << testId << " failed\n";
      return result;
   }

   return 0;
}

/* end of fut_verbosity.cpp */
