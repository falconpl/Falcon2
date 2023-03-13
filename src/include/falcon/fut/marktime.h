/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: marktime.h

  Generic Time Marker
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 14 Jan 2018 22:56:32 +0000
  Touch : Tue, 16 Jan 2018 22:29:26 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_MARKTIME_H_
#define _FALCON_MARKTIME_H_

#include <falcon/setup.h>
namespace falcon {
namespace testing {
void FALCON_API_ markTime(int64& currentMillisecs);
void FALCON_API_ waitTime(int64 millisecs);
}
}

#endif /* _FALCON_MARKTIME_H_ */

/* end of marktime.h */

