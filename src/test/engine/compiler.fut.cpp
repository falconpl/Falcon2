/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: compiler.fut.cpp

  Test for the compiler
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 20:48:25 +0000
  Touch : Sun, 17 Feb 2019 14:17:31 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <iostream>
#include <falcon/engine/compiler.h>

FALCON_TEST(Compiler, Smoke)
{
   std::string value;
   falcon::Compiler compiler;
   std::istringstream text("0");
   falcon::Code code = compiler.compile(text);
   EXPECT_STREQ("0", code.toString());
}

FALCON_TEST_MAIN

/* end of fut_checks.cpp */

