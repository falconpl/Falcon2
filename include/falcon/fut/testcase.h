/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: testcase.h

  Single test experiment
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 16:38:07 +0000
  Touch : Tue, 09 Jan 2018 23:27:02 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_TESTCASE_H_
#define _FALCON_TESTCASE_H_

#include <falcon/setup.h>

namespace Falcon {
namespace test {

/** Single test case.
 *
 * The class is meant to be statically declared at program level or main scope,
 * and will register automatically to the UnitTest singleton upon instance
 * creation.
 */
class FALCON_API_ TestCase
{
public:
   /** Creates a unit test.
    * @name A unit test name in format "Component::topic"
    * @note Will register with the UnitTest singleton upon creation.
    */
   TestCase(const char* name);
   virtual ~TestCase();

   typedef enum {
      NONE,
      SUCCESS,
      FAIL,
      ERROR,
      OUT_ON_ERROR_STREAM
   }
   t_status;

   const char* name() const;
   void status(t_status s);
   t_status status() const;
   const char* capturedOut() const;
   const char* capturedErr() const;
   const char* failDesc() const;
   const char* failFile() const;
   int failLine() const;
   void checkFail(const char* fname, int line, const char* expected, const char* actual);

   virtual void setup();
   virtual void teardown();
   void beginTest();
   void endTest();

   /* Invoked by UnitTest::main() */
   void run();

   void fail(const char* file, int line, const char* reason);

   /* Must be specialized in the final test */
   virtual void test() = 0;

private:
   class Private;
   Private *p;

};

}
}

#endif /* _FALCON_TESTCASE_H_ */

/* end of testcase.h */

