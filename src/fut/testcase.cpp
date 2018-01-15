/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: testcase.cpp

  Single test experiment
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 16:39:09 +0000
  Touch : Mon, 15 Jan 2018 00:20:14 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/testcase.h>
#include <falcon/fut/unittest.h>
#include <falcon/fut/timelapse.h>

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace Falcon {
namespace test{

class TestCase::Private
{
public:
   std::string name;
   std::string componentName;
   std::string caseName;

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

   TimeLapse lapse;
};


TestCase::TestCase():
   p(new Private)
{
   p->name = "Undefined";
   p->status = NONE;
}

TestCase::~TestCase()
{
   delete p;
}


const char* TestCase::fullName() const {
   return p->name.c_str();
}


const char* TestCase::caseName() const {
   return p->caseName.c_str();
}


const char* TestCase::componentName() const {
   return p->componentName.c_str();
}


void TestCase::setName(const char* component, const char* caseName) {
   p->componentName = component;
   p->caseName = caseName;
   p->name = p->componentName + "::" + p->caseName;
}

void TestCase::status(t_status s)
{
   p->status = s;
}


const char* TestCase::statusDesc() const {
   switch(p->status) {
   case NONE: return "NONE";
   case SUCCESS: return "SUCCESS";
   case FAIL: return "FAIL";
   case ERROR: return "EXCEMPTION";
   case OUT_ON_ERROR_STREAM: return "STREAM-ERROR";
   }
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


void TestCase::SetUp()
{
   // To be reimplemented by fixture if needed
}

void TestCase::TearDown()
{
   // To be reimplemented by fixture if needed
}


void TestCase::init()
{
   p->status = NONE;
}

void TestCase::destroy()
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

void TestCase::run()
{
   try {
      p->status = NONE;
      SetUp();

      p->lapse.markBegin();
      test();
      p->lapse.markEnd();

      if(p->status == NONE) {
         p->status = p->errCapture.str().empty() ? SUCCESS : OUT_ON_ERROR_STREAM;
      }
      TearDown();
   }
   catch(std::exception& e) {
      p->lapse.markEnd();
      p->status = ERROR;
      p->failDesc = e.what();
   }
   catch(...) {
      p->lapse.markEnd();
      p->status = ERROR;
      p->failDesc = "Unknown error caught by Falcon::test framework";
   }
}

int64 TestCase::elapsedTime() const
{
   return p->lapse.elapsed();
}

}
}

/* end of testcase.cpp */
