/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logger.fut.cpp

  Test default logger
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Thu, 28 Feb 2019 22:02:59 +0000
  Touch : Thu, 28 Feb 2019 22:43:07 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <falcon/logger.h>

#include <future>

class TestListener: public Falcon::LogSystem::Listener {
public:
	std::promise<Falcon::LogSystem::Message> m_msgPromise;

protected:
    virtual void onMessage( const Falcon::LogSystem::Message& msg ) override{
    	m_msgPromise.set_value(msg);
    }
};


class LoggerTest: public Falcon::testing::TestCase
{
public:
   void SetUp() {
      m_catcher = std::make_shared<TestListener>();
      LOG.addListener(m_catcher);
      m_caught = m_catcher->m_msgPromise.get_future();
    }

   using FutureMessage = std::future<Falcon::LogSystem::Message>;

   bool waitResult(FutureMessage& msg) {
	   std::chrono::seconds span (5);
	   if( msg.wait_for(span) == std::future_status::timeout) {
	   		FAIL("Message not received by logger");
	   		return false;
	   }
	   return true;
   }

   std::shared_ptr<TestListener> m_catcher;
   FutureMessage m_caught;

};


TEST_F(LoggerTest, Smoke)
{
   std::ostringstream tempStream;
   LOG.defaultListener()->setStream(tempStream);
   LOG << "Hello World";
   waitResult(m_caught);
   EXPECT_NE(tempStream.str().find("Hello World"), std::string::npos);
}

FALCON_TEST_MAIN

/* end of logger.fut.cpp */
