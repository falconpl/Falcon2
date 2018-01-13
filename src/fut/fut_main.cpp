/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: fut_main.cpp

  Default main() function for unit tests
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 13 Jan 2018 21:24:04 +0000
  Touch : Sat, 13 Jan 2018 21:29:09 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/unittest.h>

int main(int argc, char* argv[]) {
   return ::Falcon::test::UnitTest::singleton()->main(argc, argv);
}


/* end of fut_main.cpp */

