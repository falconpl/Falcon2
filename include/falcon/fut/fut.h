/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: fut.h

  Unit test sub-system
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 12:42:42 +0000
  Touch : Tue, 09 Jan 2018 23:27:02 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_FUT_H_
#define _FALCON_FUT_H_

#include <falcon/setup.h>
#include <falcon/fut/testcase.h>
#include <falcon/fut/unittest.h>
#include <sstream>
#include <stdexcept>

namespace Falcon {
namespace test {


}
}

#define FALCON_TEST(_COMPONENT_, _NAME_) \
   class TestCase_##_COMPONENT_##_c_##_NAME_: public ::Falcon::test::TestCase { \
      public:\
      TestCase_##_COMPONENT_##_c_##_NAME_( ):TestCase(#_COMPONENT_ "::" #_NAME_ ){}\
      virtual ~TestCase_##_COMPONENT_##_c_##_NAME_() {}\
      virtual void test(); \
      }_falcon_test_case_##_COMPONENT_##_c_##_NAME_;\
      void TestCase_##_COMPONENT_##_c_##_NAME_::test()



#define FALCON_TEST_CHECK_LOGIC(_VARIABLE_,_CHECK_,_EXPECTED_) \
		if(!( (_VARIABLE_) _CHECK_ (_EXPECTED_) )) { \
		   std::ostringstream ss; \
		   ss << (_VARIABLE_); \
			this->checkFail( __FILE__, __LINE__, #_VARIABLE_ " " #_CHECK_ " " #_EXPECTED_, ss.str().c_str() ); \
			return; \
		}

#define EXPECT_EQ( _VARIABLE_, _EXPECTED_) FALCON_TEST_CHECK_LOGIC(_VARIABLE_,==,_EXPECTED_)
#define EXPECT_NE( _VARIABLE_, _EXPECTED_) FALCON_TEST_CHECK_LOGIC(_VARIABLE_,!=,_EXPECTED_)
#define EXPECT_GT( _VARIABLE_, _EXPECTED_) FALCON_TEST_CHECK_LOGIC(_VARIABLE_,>,_EXPECTED_)
#define EXPECT_GE( _VARIABLE_, _EXPECTED_) FALCON_TEST_CHECK_LOGIC(_VARIABLE_,>=,_EXPECTED_)
#define EXPECT_LT( _VARIABLE_, _EXPECTED_) FALCON_TEST_CHECK_LOGIC(_VARIABLE_,<,_EXPECTED_)
#define EXPECT_LE( _VARIABLE_, _EXPECTED_) FALCON_TEST_CHECK_LOGIC(_VARIABLE_,<=,_EXPECTED_)

#define FAIL(_MESSAGE_) \
   this->checkFail( __FILE__, __LINE__, _MESSAGE_, "" );


#define EXPECT_THROW(_EXPRESSION_, _CATCH_) \
     try { \
         {_EXPRESSION_;}\
         std::ostringstream ss; \
         ss << "Expected exception of type " #_CATCH_ " wasn't thrown"; \
         this->checkFail( __FILE__, __LINE__, ss.str().c_str(), "" ); \
     } catch(const _CATCH_&) {}


#define EXPECT_ANY_THROW(_EXPRESSION_) \
     try { \
         {_EXPRESSION_;}\
         this->checkFail( __FILE__, __LINE__, "No throw when any throw was expected", "" ); \
     } catch(...) {}


#define EXPECT_NO_THROW(_EXPRESSION_) \
     try { \
         _EXPRESSION_; \
     } catch(const std::exception& __excpt__) { \
        std::ostringstream ss; \
        ss << "Exception thrown: \" " << __excpt__.what() << "\""; \
        this->checkFail( __FILE__, __LINE__, ss.str().c_str(), "" ); \
     } \
     catch(...) { \
        this->checkFail( __FILE__, __LINE__, "Unknown exception thrown", "" ); \
     }


#define FALCON_TEST_MAIN      \
      int main(int argc, char* argv[]) { \
         return ::Falcon::test::UnitTest::singleton()->main(argc, argv); \
      }


#endif /* _FALCON_FUT_H_ */

/* end of fut.h */

