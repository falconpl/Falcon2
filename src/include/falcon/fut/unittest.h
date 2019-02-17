/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: unittest.h

  Main unit test engine
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 16:37:43 +0000
  Touch : Tue, 16 Jan 2018 22:29:26 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_UNITTEST_H_
#define _FALCON_UNITTEST_H_

#include <falcon/setup.h>

namespace Falcon {
namespace testing {

class TestCase;

/** Unit test main engine.
 *
 * @Note the UnitTest singleton doesn't own the tests cases. It is possible
 * to register the test cases to multiple unit tests, and it's necessary to
 * dispose of them separately.
 */
class FALCON_API_ UnitTest
{
public:
   typedef enum {
      SILENT = 0,
      REPORT_FAILURE = 1,
      REPORT_STATUS = 2,
      REPORT_BEGIN = 3,
      REPORT_STDERR = 4,
      REPORT_STDOUT = 5,
   }
   t_verbosity;

   UnitTest();
   ~UnitTest();
   /** Return (and eventually create) the unit test singleton */
   static UnitTest* singleton();

   /** Used by the unit tests to add themselves */
   TestCase* addTestCase(const char* component, const char* caseName, TestCase* tcase);

   /**
    * Parse the unit test parameters and executes all the tests.
    *
    * @note After this call this object will be invalid.
    */
   int main(int argc, char* argv[]);
   void detectTestName(const char* exeName);

   int parseParams(int argc, char* argv[]);
   int performUnitTests();
   int performTest(const char* name);
   void setVerbosity( t_verbosity vb );
   void usage();

   int64 elapsedTime() const;

private:
   class Private;
   Private* p;

   void init();
   void runAllTests();
   void report();
   void destroy();
   void writeTestName(int count, const char* tname, const char* result, double elapsed=-1);
   void beginTest(int count, TestCase* tc);
   void endTest(int count, TestCase* tc);
   void saveToXML() const;
   

   bool hasPassed(TestCase* tc) const;
};

}
}

#endif /* _FALCON_UNITTEST_H_ */

/* end of unittest.h */

