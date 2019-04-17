/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: futex.cpp

  Lightweight non-blocking mutex
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Fri, 12 Apr 2019 19:19:08 +0100
  Touch : Fri, 12 Apr 2019 19:19:08 +0100

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/futex.h>

namespace Falcon {
std::atomic<unsigned int> RFutex::s_count{0};
thread_local unsigned int RFutex::s_thread_id{0};
}

/* end of futex.cpp */
