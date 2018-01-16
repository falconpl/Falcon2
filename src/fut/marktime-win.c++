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
#include <windows.h>

namespace Falcon {
namespace testing {

void FALCON_API_ markTime(int64& currentMillisecs)
{
   SYSTEMTIME now;
   GetSystemTime(&now);
   currentMillisecs = now.wHour * 3600 * 1000 +
		now.wMinute * 60 * 1000 +
		now.wSecond * 1000 +
		now.wMilliseconds;
}

void FALCON_API_ waitTime(int64 millsecs) {
	Sleep((DWORD)millsecs);
}

}
}

/* end of marktime-win.cpp */

