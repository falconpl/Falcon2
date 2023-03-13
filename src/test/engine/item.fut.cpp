/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: singleton.fut.cpp

  Test the Item class
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 23 Feb 2019 05:21:08 +0000
  Touch : Thu, 28 Feb 2019 22:43:30 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <string>

#include "falcon/engine/item.h"


namespace falcon {

FALCON_TEST(Item, Smoke)
{
	Item item(0LL);
  EXPECT_EQ(0, item.toInt());
}

FALCON_TEST(Item, StringItem)
{
	Item item("Hello world");
  EXPECT_EQ("Hello world", item.toString());
}

FALCON_TEST(Item, StringBool)
{
  EXPECT_TRUE(Item("abc").toBool());
  EXPECT_FALSE(Item("").toBool());
}

FALCON_TEST(Item, StringNumber)
{
  EXPECT_EQ(100LL, Item("100").toInt());
}

}

/* end of singleton.fut.cpp */
