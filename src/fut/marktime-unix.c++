/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: marktime-unix.cpp

  Unix-specific time marker
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 14 Jan 2018 22:57:34 +0000
  Touch : Mon, 15 Jan 2018 00:20:14 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/fut/marktime.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdexcept>

namespace Falcon {
namespace testing {

void markTime(int64& currentMillisecs)
{
   struct timeval tv;
   if(gettimeofday(&tv, NULL)) {
      throw std::runtime_error("gettimeofday failed");
   }
   currentMillisecs = (tv.tv_sec * 1000);
   currentMillisecs += (tv.tv_usec / 1000);
}

void waitTime(int64 mseconds) {
	usleep(mseconds * 1000);
}

}
}


/* end of marktime-unix.cpp */
