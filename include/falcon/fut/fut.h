/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: fut.h

  Unit test sub-system
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 12:42:42 +0000
  Touch : Sun, 14 Jan 2018 20:24:55 +0000

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
#include <string>
#include <stdexcept>


#define FALCON_TEST_CLASS_NAME(_COMPONENT_, _NAME_) \
      TestCase_##_COMPONENT_##_c_##_NAME_

#define _FALCON_TEST_DECLARE_CLASS_(_CLASSNAME_,_PARENTNAME_,_TNAME_) \
   class _CLASSNAME_: public _PARENTNAME_ { \
      public:\
      _CLASSNAME_( ):_PARENTNAME_(){}\
      virtual ~_CLASSNAME_() {}\
      virtual void test(); \
      static _CLASSNAME_* _instance_;\
      };\
      _CLASSNAME_* _CLASSNAME_::_instance_ = \
            static_cast<_CLASSNAME_*>(::Falcon::test::UnitTest::singleton()->\
               addTestCase(_TNAME_,new _CLASSNAME_));\
      void _CLASSNAME_::test()

#define FALCON_TEST(_COMPONENT_, _NAME_) \
    _FALCON_TEST_DECLARE_CLASS_(FALCON_TEST_CLASS_NAME(_COMPONENT_,_NAME_),::Falcon::test::TestCase, #_COMPONENT_ "::" #_NAME_  )

#define FALCON_TEST_F(_COMPONENT_, _NAME_) \
    _FALCON_TEST_DECLARE_CLASS_(FALCON_TEST_CLASS_NAME(_COMPONENT_,_NAME_),_COMPONENT_, #_COMPONENT_ "::" #_NAME_  )

#ifndef FALCON_TEST_DONT_DEFINE_TEST
#define TEST   FALCON_TEST
#define TEST_F FALCON_TEST_F
#endif

#define FALCON_TEST_CHECK_LOGIC(_EXPECTED_,_CHECK_,_ACTUAL_) \
		if(!( (_EXPECTED_) _CHECK_ (_ACTUAL_) )) { \
		   std::ostringstream ss; \
		   ss << (_ACTUAL_); \
			this->checkFail( __FILE__, __LINE__, #_EXPECTED_ " " #_CHECK_ " " #_ACTUAL_, ss.str().c_str() ); \
			return; \
		}

#define EXPECT_EQ( _EXPECTED_, _ACTUAL_) FALCON_TEST_CHECK_LOGIC(_EXPECTED_,==,_ACTUAL_)
#define EXPECT_NE( _EXPECTED_, _ACTUAL_) FALCON_TEST_CHECK_LOGIC(_EXPECTED_,!=,_ACTUAL_)
#define EXPECT_GT( _EXPECTED_, _ACTUAL_) FALCON_TEST_CHECK_LOGIC(_EXPECTED_,>,_ACTUAL_)
#define EXPECT_GE( _EXPECTED_, _ACTUAL_) FALCON_TEST_CHECK_LOGIC(_EXPECTED_,>=,_ACTUAL_)
#define EXPECT_LT( _EXPECTED_, _ACTUAL_) FALCON_TEST_CHECK_LOGIC(_EXPECTED_,<,_ACTUAL_)
#define EXPECT_LE( _EXPECTED_, _ACTUAL_) FALCON_TEST_CHECK_LOGIC(_EXPECTED_,<=,_ACTUAL_)
#define EXPECT_SEQ( _EXPECTED_, _ACTUAL_) FALCON_TEST_CHECK_LOGIC(std::string(_EXPECTED_),==,std::string(_ACTUAL_))

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
         return ::Falcon::test::UnitTest::singleton()->main(argc-1, argv+1); \
      }


#endif /* _FALCON_FUT_H_ */

/* end of fut.h */

