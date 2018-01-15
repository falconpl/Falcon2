/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: marktime.h

  Generic Time Marker
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 14 Jan 2018 22:56:32 +0000
  Touch : Mon, 15 Jan 2018 00:20:14 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_MARKTIME_H_
#define _FALCON_MARKTIME_H_

#include <falcon/setup.h>
namespace Falcon {
namespace testing {
void markTime(int64& currentMillisecs);
}
}

#endif /* _FALCON_MARKTIME_H_ */

/* end of marktime.h */

