/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: timelapse.h

  A simple structure to handle timelapses
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 14 Jan 2018 23:15:56 +0000
  Touch : Mon, 15 Jan 2018 00:20:14 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_TIMELAPSE_H_
#define _FALCON_TIMELAPSE_H_

#include <falcon/setup.h>
#include <falcon/fut/marktime.h>

namespace Falcon {
namespace test{

class TimeLapse{
public:
   enum {
        MSECS_IN_A_DAY = 60*60*24*1000
     };

   TimeLapse():
      m_begin(0),
      m_end(0)
   {}

   int64 elapsed() const{
      return m_begin > m_end
            ? m_end - m_begin + MSECS_IN_A_DAY
            : m_end - m_begin;
   }

   int64 markBegin() { markTime(m_begin); return m_begin; }
   int64 markEnd() { markTime(m_end); return m_end; }

   int64 begin() const {return m_begin;}
   int64 end() const {return m_end;}

public:
   int64 m_begin;
   int64 m_end;
};
}
}

#endif /* _FALCON_TIMELAPSE_H_ */

/* end of timelapse.h */

