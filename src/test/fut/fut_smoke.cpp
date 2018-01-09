/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: fut_smoke.cpp

  Smoke tests for the Unit Test System
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 09 Jan 2018 14:33:58 +0000
  Touch : Tue, 09 Jan 2018 23:27:02 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/fut.h>

#include <iostream>
#include <stdexcept>

FALCON_TEST(FUT, smoke) {
   std::cout << "Hello world!" << std::endl;
}

FALCON_TEST_MAIN

/* end of fut_smoke.cpp */

