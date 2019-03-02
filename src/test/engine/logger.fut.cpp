/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logger.fut.cpp

  Test default logger
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Thu, 28 Feb 2019 22:02:59 +0000
  Touch : Sat, 02 Mar 2019 11:12:05 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <falcon/logger.h>

#include <iostream>

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
      LOGGER.addListener(m_catcher);
      LOGGER.defaultListener()->writeOn(&m_sstream);
      m_caught = m_catcher->m_msgPromise.get_future();
    }

   void TearDown() {
	   // Detach the catcher stream, or it will still be there
	   // the next time we use this fixture!
	   m_catcher->detach();
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
   std::stringstream m_sstream;

};


TEST_F(LoggerTest, Smoke)
{
   LOGGER << "Hello World";
   LOGGER.commit();
   waitResult(m_caught);
   EXPECT_NE(m_sstream.str().find("Hello World"), std::string::npos);
}

TEST_F(LoggerTest, AutoEnd)
{
   LOG_INFO << "Hello Auto";
   waitResult(m_caught);
   EXPECT_NE(m_sstream.str().find("Hello Auto"), std::string::npos);
}


TEST_F(LoggerTest, Category)
{
   LOG_CATEGORY("The Category");
   LOG_INFO << "Hello Category";
   waitResult(m_caught);
   EXPECT_NE(m_sstream.str().find("The Category"), std::string::npos);
}


TEST_F(LoggerTest, LogBlock)
{
   LOG_BLOCK_INFO {
	   LOGGER << "Hello";
	   LOGGER << " partial";
	   LOGGER << " world";
   }
   waitResult(m_caught);
   std::cout << m_sstream.str();
   EXPECT_NE(m_sstream.str().find("Hello partial world"), std::string::npos);
}

FALCON_TEST_MAIN

/* end of logger.fut.cpp */
