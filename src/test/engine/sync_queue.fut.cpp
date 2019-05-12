/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: sync_queue.fut.cpp

  Sychronised Queue
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 04 May 2019 19:42:21 +0100
  Touch : Sat, 04 May 2019 19:42:21 +0100

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <falcon/engine/syncqueue.h>
#include <string>
#include <thread>
#include <vector>
#include <iostream>

using namespace Falcon;
using namespace std::chrono_literals;

class SyncQueueTest: public testing::TestCase
{
public:
   using squeue = sync_queue<std::string>;
   squeue m_messages;
   std::vector<std::string> to_send{"Hello", "World", "More", "Stuff"};

   void SetUp() {
   }

   void TearDown() {
   }

   SyncQueueTest(){}


   auto make_listener(std::vector<squeue::value_type>& results) {
      return [&]() {
         std::string received;
         while((received = m_messages.get())!="") {
            std::cout << "Received: "<< received << "\n";
            results.push_back(received);
         }
         std::cout << "Exiting with: \""<< received << "\"\n";
      };
   }

   template<typename _r, typename _p>
   void send_messages(std::chrono::duration<_r,_p> d) {
      for(auto& value: to_send) {
         std::cout << "Sending with: \"" << value << "\"\n";
         std::this_thread::sleep_for(d);
         m_messages.push(value);
      }
   }

   void check_messages(std::vector<squeue::value_type>& results)
   {
      EXPECT_EQ(to_send.size(), results.size());
      for(int i = 0; i < to_send.size(); ++i)
      {
         EXPECT_EQ(to_send[i], results[i]);
      }
   }
};


TEST_F(SyncQueueTest, push)
{
   m_messages.push("Hello world");
   std::string hw = m_messages.get();
   EXPECT_EQ("Hello world", hw);
}

TEST_F(SyncQueueTest, emplace)
{
   m_messages.emplace("Hello world");
   std::string hw = m_messages.get();
   EXPECT_EQ("Hello world", hw);
}

TEST_F(SyncQueueTest, timeout)
{
   auto option = m_messages.get(1ms);
   EXPECT_FALSE(option.has_value());
}


TEST_F(SyncQueueTest, no_timeout)
{
   m_messages.emplace("Hello world");
   auto option = m_messages.get(1ms);

   EXPECT_TRUE(option.has_value());
   EXPECT_EQ("Hello world", *option);
}


TEST_F(SyncQueueTest, mt)
{
   std::vector<squeue::value_type> results;
   std::thread parallel(make_listener(results));
   send_messages(0ms);
   m_messages.push("");
   parallel.join();
   check_messages(results);
}

TEST_F(SyncQueueTest, mt_delay)
{
   std::vector<squeue::value_type> results;
   std::thread parallel(make_listener(results));
   send_messages(5ms);
   m_messages.push("");
   parallel.join();
   check_messages(results);
}

/* end of sync_queue.fut.cpp */

