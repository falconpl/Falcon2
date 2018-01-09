/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: fut_checks.cpp

  Test the standard (successful) tests
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 20:48:25 +0000
  Touch : Tue, 09 Jan 2018 23:27:02 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>
#include <iostream>

FALCON_TEST(EXPECT, BasicPositive)
{
   std::string value;

   value = "Hello";
   EXPECT_EQ("Hello", value);
   EXPECT_NE("World", value);

   int ivalue = 10;
   EXPECT_GT(ivalue, 5);
   EXPECT_GE(ivalue, 5);
   EXPECT_GE(ivalue, 10);

   EXPECT_LT(ivalue, 20);
   EXPECT_LE(ivalue, 20);
   EXPECT_LE(ivalue, 10);
}

FALCON_TEST(EXPECT, ThrowPositive)
{
   EXPECT_THROW({
      throw std::runtime_error("Something");
   },
   std::runtime_error);


   EXPECT_ANY_THROW({
      throw std::runtime_error("Something");
   });

   EXPECT_NO_THROW({
      int value = 10;
      value =+ 1;
      std::cout << value <<"\n";
   });
}


FALCON_TEST_MAIN

/* end of fut_checks.cpp */

