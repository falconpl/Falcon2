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
	int m_expected{1};

protected:
    virtual void onMessage( const Falcon::LogSystem::Message& msg ) override{
    	if (--m_expected == 0) {
    		m_msgPromise.set_value(msg);
    	}
    }
};


class LoggerTest: public Falcon::testing::TestCase
{
public:
   void SetUp() {
	   resetCatcher();
       LOGGER.defaultListener()->writeOn(&m_sstream);
   }

   void resetCatcher() {
	   if(m_catcher.get()) {
		   m_catcher->detach();
	   }
	   m_catcher = std::make_shared<TestListener>();
	   m_caught = m_catcher->m_msgPromise.get_future();
	   LOGGER.addListener(m_catcher);
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

   bool same_line(const std::string& lines, const std::string& one, const std::string& two) {
	   std::stringstream ss(lines);
	   std::string to;

	   while(std::getline(ss, to, '\n')){
		   if(to.find(one) != std::string::npos && to.find(two) != std::string::npos) {
			   return true;
		   }
	   }
	   return false;
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

TEST_F(LoggerTest, TempCategory)
{
   // We expect TempCat to be before One, and Final Category after One.
	// Be sure not to break searches.
   LOG_CATEGORY("Cat");
   LOG_INFO << LOG_CAT("Temp") << "One";
   LOG_INFO << "Two";

   waitResult(m_caught);

   const std::string& str = m_sstream.str();
   std::string::size_type posCat{str.find("Cat")},
   	   posTempCat{str.find("Temp")},
	   posOne{str.find("One")},
	   rposTemp{str.rfind("Temp")};

	// Initial sanity check
    EXPECT_NE(std::string::npos, posCat);
    EXPECT_NE(std::string::npos, posTempCat);
    EXPECT_NE(std::string::npos, posOne);
    EXPECT_NE(std::string::npos, rposTemp);

    // We want only one TEMP and one CAT in the log
    EXPECT_EQ(rposTemp, posTempCat);

    EXPECT_LT(posTempCat, posOne);
    EXPECT_LT(posTempCat, posCat);
    EXPECT_LT(posOne, posCat);
}


TEST_F(LoggerTest, LogBlock)
{
   LOG_BLOCK_INFO {
	   LOGGER << "Hello";
	   LOGGER << " partial";
	   LOGGER << " world";
   }
   waitResult(m_caught);
   EXPECT_NE(m_sstream.str().find("Hello partial world"), std::string::npos);
}


TEST_F(LoggerTest, CategoryFilter)
{
	// We need to wait for 4 log lines in this test
	m_catcher->m_expected = 3;
	// we should not receive anything under info
	LOGGER.level(Falcon::LLINFO);
	LOGGER.defaultListener()->level(Falcon::LLINFO);
	LOGGER.categoryFilter(".*::INTERNAL", Falcon::LLTRACE);

    LOG_CATEGORY("Test::BASE");
	LOG_INFO << "Line INFO";
	LOG_TRC << "Line TRACE";

	LOG_CATEGORY("Test::INTERNAL");
	LOG_TRC << "Line INTERNAL";

	waitResult(m_caught);
	resetCatcher();

	LOGGER.clearFilter();
	LOGGER.level(Falcon::LLTRACE);
	LOG_TRC << "Line FINAL";
	waitResult(m_caught);

	std::cout << m_sstream.str();
	// Line INFO must be in the same line of Test::BASE
	// Line TRACE is expected not to be printed
	// Line INTERNAL is expected on Test::INTERNAL to be received.
	// Line FINAL should be not received.
	EXPECT_TRUE(same_line(m_sstream.str(), "Test::BASE", "Line INFO"));
	EXPECT_FALSE(same_line(m_sstream.str(), "Test::BASE", "Line TRACE"));
	EXPECT_TRUE(same_line(m_sstream.str(), "Test::INTERNAL", "Line INTERNAL"));
	EXPECT_FALSE(same_line(m_sstream.str(), "Test::INTERNAL", "Line FINAL"));
}

FALCON_TEST_MAIN

/* end of logger.fut.cpp */
