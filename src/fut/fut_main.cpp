/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: fut_main.cpp

  Default main() function for unit tests
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 13 Jan 2018 21:24:04 +0000
  Touch : Sun, 14 Jan 2018 20:24:55 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/unittest.h>
#include <cassert>

extern "C" {
	int main(int argc, char* argv[]) {
		assert(argc > 0);
		return ::Falcon::testing::UnitTest::singleton()->main(argc - 1, argv + 1);
	}
}

/* end of fut_main.cpp */

