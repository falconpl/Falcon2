/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: atomic.h

  Atomic operations
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 02 Jan 2018 16:41:16 +0000
  Touch : Tue, 02 Jan 2018 18:31:24 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_ATOMIC_H_
#define _FALCON_ATOMIC_H_

#include <falcon/setup.h>

#if defined(FALCON_SYSTEM_WIN) && !defined(__MINGW32__)
#include <falcon/atomic_vc.h>
#else
// TODO: atomic on other platforms
#include <falcon/atomic_gcc.h>
#endif

#endif /* _FALCON_ATOMIC_H_ */

/* end of atomic.h */
