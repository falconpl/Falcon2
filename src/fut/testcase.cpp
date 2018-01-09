/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: testcase.cpp

  Single test experiment
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 16:39:09 +0000
  Touch : Tue, 09 Jan 2018 23:27:02 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/testcase.h>
#include <falcon/fut/unittest.h>

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace Falcon {
namespace test{

class TestCase::Private
{
public:
   const char* name;
   TestCase::t_status status;
   std::ostringstream outCapture;
   std::ostringstream errCapture;
   std::string outCaptureStr;
   std::string errCaptureStr;

   std::streambuf *outbuf;
   std::streambuf *errbuf;

   std::string failDesc;
   int failLine;
   std::string failFile;
};

TestCase::TestCase(const char* name):
   p(new Private)
{
   p->name = name;
   p->status = NONE;
   UnitTest::singleton()->addTestCase(this);
}

TestCase::~TestCase()
{
   delete p;
}

const char* TestCase::name() const {
   return p->name;
}

void TestCase::status(t_status s)
{
   p->status = s;
}


void TestCase::fail(const char* file, int line, const char* reason)
{
   p->status = FAIL;
   p->failFile = file;
   p->failLine = line;
   p->failDesc = reason;
}

TestCase::t_status TestCase::status() const
{
   return p->status;
}

const char* TestCase::capturedOut() const
{
   return p->outCaptureStr.c_str();
}

const char* TestCase::capturedErr() const
{
   return p->errCaptureStr.c_str();
}

const char* TestCase::failDesc() const
{
   return p->failDesc.c_str();
}

const char* TestCase::failFile() const
{
   return p->failFile.c_str();
}

int TestCase::failLine() const
{
   return p->failLine;
}


void TestCase::checkFail(const char* fname, int line, const char* expected, const char* actual)
{
   p->failDesc = expected;
   if(*actual) {
      p->failDesc += " (Actually \"";
      p->failDesc += actual;
      p->failDesc += "\")";
   }

   p->failLine = line;
   p->failFile = fname;
   //strip path
   std::string::size_type pos = p->failFile.rfind(FALCON_DIR_SEP_CHR);
   if(pos != std::string::npos) {
      p->failFile = p->failFile.substr(pos+1);
   }
   p->status = FAIL;
}


void TestCase::setup()
{
   p->status = NONE;
}

void TestCase::teardown()
{
   //TODO
}

void TestCase::beginTest()
{
   // saving the standard buffers
   p->outbuf = std::cout.rdbuf();
   p->errbuf = std::cerr.rdbuf();
   std::cout.rdbuf(p->outCapture.rdbuf());
   std::cerr.rdbuf(p->errCapture.rdbuf());

   p->status = NONE;
}

void TestCase::endTest()
{
   // When done redirect cout to its old self
   std::cout.rdbuf(p->outbuf);
   std::cerr.rdbuf(p->errbuf);

   p->outCaptureStr = p->outCapture.str();
   p->errCaptureStr = p->errCapture.str();
}

void TestCase::run() {
   try {
      p->status = NONE;

      test();

      if(p->status == NONE) {
         p->status = p->errCapture.str().empty() ? SUCCESS : OUT_ON_ERROR_STREAM;
      }
   }
   catch(std::exception& e) {
      p->status = ERROR;
      p->failDesc = e.what();
   }
   catch(...) {
      p->status = ERROR;
      p->failDesc = "Unknown error caught by Falcon::test framework";
   }
}

}
}


/* end of testcase.cpp */

