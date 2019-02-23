/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: singleton.fut.cpp

  Test the Singleton class
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 23 Feb 2019 05:21:08 +0000
  Touch : Sat, 23 Feb 2019 07:15:20 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <string>
#include <falcon/engine/singleton.h>


namespace Falcon {
Singleton<std::string> theString;

FALCON_TEST(Compiler, Smoke)
{
	*theString.mtbl = "Hello";
	EXPECT_STREQ("Hello", *theString);
	EXPECT_STRNE("Something Else", *theString);
	EXPECT_EQ(*theString, *theString.get());
}

}

/* end of singleton.fut.cpp */
