/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: fut.h

  Unit test sub-system
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 12:42:42 +0000
  Touch : Tue, 09 Jan 2018 15:30:48 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_FUT_H_
#define _FALCON_FUT_H_

#include <falcon/setup.h>

#define FALCON_API_

namespace Falcon {
namespace test {

class UnitTest;

class FALCON_API_ TestCase
{
public:
	TestCase(const char* name);
	virtual ~TestCase();

	virtual void run() = 0;

	void setResult( bool result) {
	   m_success = result;
	}

	bool isSuccess() const { return m_success;}
   bool isFailure() const { return ! isSuccess();}

   virtual void setup();
   virtual void teardown();

private:
	const char* m_name;

	bool m_complete;
	bool m_success;
	friend class UnitTest;

	void beginTest();
	void endTest();
};


class FALCON_API_ UnitTest
{
public:
   UnitTest();
   ~UnitTest();
   static UnitTest* singleton();

   void addTestCase(TestCase* tcase);
   int main(int argc, char* argv[]);

private:
   class Private;
   Private* p;

   void setup();
   void runAllTests();
   void report();
   void tearDown();
};

}
}

#define FALCON_TEST(_COMPONENT_, _NAME_) \
   class TestCase_##_COMPONENT_##_c_##_NAME_: public ::Falcon::test::TestCase { \
      public:\
      TestCase_##_COMPONENT_##_c_##_NAME_( ):TestCase(#_COMPONENT_ "::" #_NAME_ ){}\
      virtual ~TestCase_##_COMPONENT_##_c_##_NAME_() {}\
      virtual void run(); \
      }_falcon_test_case_##_COMPONENT_##_c_##_NAME_;\
      void TestCase_##_COMPONENT_##_c_##_NAME_::run()


/*
#define FALCON_TEST_CHECK_LOGIC( _EXPECTED_, _VARIABLE_, _CHECK_, _CHECK_NAME_ ) 	\
		if( (_EXPECTED_) _CHECK_ (_VARIABLE_) ) {
			_falcon_test_success( __FILE__, __LINE__, _FALCON_TEST_NAME_, _EXPECTED_, _VARIABLE_, _CHECK_NAME_ );
		}
		else {
			_falcon_test_failure( __FILE__, __LINE__, _FALCON_TEST_NAME_, _EXPECTED_, _VARIABLE_, _CHECK_NAME_ );
		}
*/

#define FALCON_TEST_MAIN      \
      int main(int argc, char* argv[]) { \
         return ::Falcon::test::UnitTest::singleton()->main(argc, argv); \
      }


#endif /* _FALCON_FUT_H_ */

/* end of fut.h */

