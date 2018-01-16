/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: fut_timing.cpp

  Check for correct timing taking
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Mon, 15 Jan 2018 20:30:45 +0000
  Touch : Tue, 16 Jan 2018 22:31:22 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/
#include <falcon/fut/fut.h>
#include <falcon/fut/marktime.h>

namespace Falcon {

class CAT1: public testing::TestCase
{
};

class CAT2: public testing::TestCase
{
};

class CAT3: public testing::TestCase
{
};

#define TIMETEST(_COMPONENT_,_TEST_, _TIME_) \
   TEST_F(_COMPONENT_, _TEST_)\
   {\
      testing::waitTime(_TIME_);\
      int64 elapsedNow = this->elapsedTimeNow();\
      EXPECT_LT(_TIME_-50, elapsedNow);\
      EXPECT_GT(_TIME_+250, elapsedNow);\
   }

TIMETEST(CAT1,Time1, 200)
TIMETEST(CAT2,Time1, 200)
TIMETEST(CAT3,Time1, 200)

TIMETEST(CAT1,Time2, 250)
TIMETEST(CAT2,Time2, 250)
TIMETEST(CAT3,Time2, 250)

TIMETEST(CAT1,Time3, 150)
TIMETEST(CAT2,Time3, 150)
TIMETEST(CAT3,Time3, 150)

}

FALCON_TEST_MAIN

/* end of fut_timing.cpp */

