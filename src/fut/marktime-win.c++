/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: marktime-win.cpp

  Windows-specific time marker
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 14 Jan 2018 22:57:25 +0000
  Touch : Mon, 15 Jan 2018 00:20:14 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/marktime.h>

namespace Falcon {
namespace test {

void markTime(int64& currentMillisecs)
{
  currentMillisecs = 0;
}

}
}

/* end of marktime-win.cpp */

