/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logsystem.fut.cpp

  Test for the log system
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 24 Feb 2019 10:15:41 +0000
  Touch : Thu, 28 Feb 2019 22:43:30 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <falcon/logsystem.h>

#include <future>
#include <iostream>
#include <memory>
#include <stdexcept>



class TestListener: public falcon::LogSystem::Listener {
public:
	std::promise<falcon::LogSystem::Message> m_msgPromise;

protected:
    virtual void onMessage( const falcon::LogSystem::Message& msg ) override{
    	m_msgPromise.set_value(msg);
    }
};


class LogTest: public falcon::testing::TestCase
{
public:
   void SetUp() {
      m_log = std::make_unique<falcon::LogSystem>();
      m_listener = std::make_shared<TestListener>();
      m_catcher = std::make_shared<TestListener>();
      m_log->addListener(m_listener);
      m_log->addListener(m_catcher);
      m_incoming = m_listener->m_msgPromise.get_future();
      m_caught = m_catcher->m_msgPromise.get_future();
    }

   void TearDown() {
	   m_log->stop();
   }

   void sendLog(falcon::LogSystem::LEVEL level, const std::string& category) {
		m_log->log("File", 101, level, category, "Message");
   }

   using FutureMessage = std::future<falcon::LogSystem::Message>;

   bool waitResult(FutureMessage& msg) {
	   std::chrono::seconds span (5);
	   if( msg.wait_for(span) == std::future_status::timeout) {
	   		FAIL("Message not received by logger");
	   		return false;
	   }
	   return true;
   }

   void sanityCheck(size_t enabled=2, size_t msgs=1) {
	   // Stop waits for the log thread to be stopped, so  we know
	   // we have a stable situation on the message queues.
	    m_log->stop();
		falcon::LogSystem::Diags diags;
		m_log->getDiags(diags);

		EXPECT_EQ(falcon::LogSystem::MESSAGE_POOL_THRESHOLD, diags.m_poolSize);
		EXPECT_EQ(2, diags.m_activeListeners);
		EXPECT_EQ(enabled, diags.m_enabledListeners);
		EXPECT_EQ(0, diags.m_pendingListeners);
		EXPECT_EQ(msgs, diags.m_msgReceived);
		EXPECT_EQ(0, diags.m_msgsCreated);
		EXPECT_EQ(0, diags.m_msgsDiscarded);

		EXPECT_LE(msgs, diags.m_maxMsgQueueSize);
   }

   std::unique_ptr<falcon::LogSystem> m_log;
   std::shared_ptr<TestListener> m_listener;
   std::shared_ptr<TestListener> m_catcher;
   FutureMessage m_incoming;
   FutureMessage m_caught;

};


TEST_F(LogTest, Smoke) {
	falcon::LogSystem::Diags diags;
	m_log->getDiags(diags);

	EXPECT_EQ(falcon::LogSystem::MESSAGE_POOL_THRESHOLD, diags.m_poolSize);
	EXPECT_EQ(0, diags.m_activeListeners);
	EXPECT_EQ(0, diags.m_enabledListeners);
	EXPECT_EQ(2, diags.m_pendingListeners);
	EXPECT_EQ(0, diags.m_msgReceived);
	EXPECT_EQ(0, diags.m_msgsCreated);
	EXPECT_EQ(0, diags.m_msgsDiscarded);

	EXPECT_EQ(0, diags.m_maxMsgQueueSize);
}


TEST_F(LogTest, ReceiveMessage) {
	// Send the log
	sendLog(falcon::LogSystem::LEVEL::INFO, "Category");

	// Wait for the message to be received.
	if(waitResult(m_incoming)) {
		//Checks:
		falcon::LogSystem::Message msg = m_incoming.get();
		EXPECT_STREQ("File", msg.m_file);
		EXPECT_EQ(101, msg.m_line);
		EXPECT_EQ(falcon::LogSystem::LEVEL::INFO, msg.m_level);
		EXPECT_STREQ("Category", msg.m_category);
		EXPECT_STREQ("Message", msg.m_message);
		sanityCheck();
	}
}

TEST_F(LogTest, ThrowCategory) {
	EXPECT_THROW(
			m_listener->category("*"),
			falcon::LogSystem::InvalidCategory
	);
}

TEST_F(LogTest, DiscardLevel) {
	// We now check that a listener is not sent messages
	// it is not interested in (by level).

	m_listener->level(falcon::LogSystem::LEVEL::DEBUG);
	sendLog(falcon::LogSystem::LEVEL::TRACE, "Category");

	// Wait for the message to be received.
	if(waitResult(m_caught)) {
		if(m_incoming.wait_for(std::chrono::seconds(0)) != std::future_status::timeout) {
			FAIL("The message was not filtered out on lower log level");
		}
		else {
			sanityCheck();
		}
	}
}


TEST_F(LogTest, DiscardCategory) {
	//Listener with a non-matching category should be ignored.
	m_listener->category("SomeCategory");
	sendLog(falcon::LogSystem::LEVEL::CRITICAL, "Category");

	// Wait for the message to be received.
	if(waitResult(m_caught)) {
		if(m_incoming.wait_for(std::chrono::seconds(0)) != std::future_status::timeout) {
			FAIL("The message was not filtered out on different category");
		}
		else {
			sanityCheck();
		}
	}
}


TEST_F(LogTest, ListenerDisabled) {
	// Disabled listeners must not receive messages.
	m_listener->enable(false);
	sendLog(falcon::LogSystem::LEVEL::TRACE, "Category");

	// Wait for the message to be received.
	if(waitResult(m_caught)) {
		if(m_incoming.wait_for(std::chrono::seconds(0)) != std::future_status::timeout) {
			FAIL("The message was sent to a disabled listener");
		}
		else {
			sanityCheck(1,1);
		}
	}
}


TEST_F(LogTest, RightCategory) {
	// Be sure that a listener gets intended categories
	m_listener->category("Good.*");
	sendLog(falcon::LogSystem::LEVEL::INFO, "GoodCategory");

	// Wait for the message to be received.
	if(waitResult(m_incoming)) {
		//Checks:
		falcon::LogSystem::Message msg = m_incoming.get();
		EXPECT_STREQ("GoodCategory", msg.m_category);
		sanityCheck();
	}
}


FALCON_TEST_MAIN


/* end of logsystem.fut.cpp */
