/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logstream.fut.cpp

  Test for the stream listener for the logging system
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 02 Mar 2019 00:23:13 +0000
  Touch : Sat, 02 Mar 2019 01:33:45 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <falcon/logsystem.h>
#include <falcon/logstream.h>

#include <future>
#include <iostream>
#include <memory>
#include <sstream>

#define MESSAGE_DATA "Message"

class TestListener: public Falcon::LogSystem::Listener {
public:
	std::promise<Falcon::LogSystem::Message> m_msgPromise;

protected:
    virtual void onMessage( const Falcon::LogSystem::Message& msg ) override{
    	m_msgPromise.set_value(msg);
    }
};


class LogStreamTest: public Falcon::testing::TestCase
{
public:
   void SetUp() {
      m_log = std::make_unique<Falcon::LogSystem>();
      m_slst = std::make_shared<Falcon::LogStreamListener>();
      m_catcher = std::make_shared<TestListener>();
      m_log->addListener(m_slst);
      m_log->addListener(m_catcher);
      m_caught = m_catcher->m_msgPromise.get_future();
   }

   void sendLog() {
		m_log->log("File", 101, Falcon::LogSystem::LEVEL::INFO, "", MESSAGE_DATA);
   }

   bool waitResult() {
	   std::chrono::seconds span (5);
	   if( m_caught.wait_for(span) == std::future_status::timeout) {
	   		FAIL("Message not received by logger");
	   		return false;
	   }
	   return true;
   }

   void test_shared() {
		auto out = std::make_shared<std::ostringstream>();
		m_slst->share(out);
		sendLog();
		std::cout << out->str() ;
		waitResult();
		EXPECT_NE(std::string::npos, out->str().find(MESSAGE_DATA));
   }

   void test_passOwn() {
		// yeah, send as unique, but actually keep a copy of the raw ptr...
		auto out = std::unique_ptr<std::ostream>(new std::ostringstream);
		auto os = static_cast<std::ostringstream*>(out.get());
		m_slst->own(out);
		sendLog();
		waitResult();
		// ... as we are sure the unique copy will still be alive here.
		EXPECT_NE(std::string::npos, os->str().find(MESSAGE_DATA));
   }

   void test_writeOn() {
		std::ostringstream out;
		m_slst->writeOn(&out);
		sendLog();
		waitResult();
		// ... as we are sure the unique copy will still be alive here.
		EXPECT_NE(std::string::npos, out.str().find(MESSAGE_DATA));
   }

   std::unique_ptr<Falcon::LogSystem> m_log;
   std::shared_ptr<Falcon::LogStreamListener> m_slst;
   std::shared_ptr<TestListener> m_catcher;
   std::future<Falcon::LogSystem::Message> m_caught;

};


TEST_F(LogStreamTest, Smoke) {
	sendLog();
	waitResult();
	EXPECT_EQ(MESSAGE_DATA, m_caught.get().m_message);
}


TEST_F(LogStreamTest, SharedStream) {
	test_shared();
}


TEST_F(LogStreamTest, UniqueStream) {
	test_passOwn();
}


TEST_F(LogStreamTest, WriteOnStream) {
	test_writeOn();
}


TEST_F(LogStreamTest, ChangeStream) {
	// Check the changing stream.
	auto shout = std::make_shared<std::ostringstream>();
	m_slst->share(shout);
	auto uout = std::unique_ptr<std::ostream>(new std::ostringstream);
	m_slst->own(uout);
	std::ostringstream out;
	m_slst->writeOn(&out);
	sendLog();
	waitResult();
	// ... as we are sure the unique copy will still be alive here.
	EXPECT_NE(std::string::npos, out.str().find(MESSAGE_DATA));
}


/* end of logstream.fut.cpp */

