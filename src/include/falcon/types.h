/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: types.h

  Basic types definitions
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 02 Jan 2018 16:02:42 +0000
  Touch : Tue, 02 Jan 2018 18:31:24 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_TYPES_H_
#define _FALCON_TYPES_H_

// Inclusion of stddef for size_t
#include <stddef.h>
#include <sys/types.h>

namespace Falcon {

typedef char* cstring;

typedef unsigned char byte;
typedef unsigned char* bytearray;

typedef byte uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;

#ifdef _MSC_VER
typedef unsigned __int64 uint64;
#else
typedef unsigned long long int uint64;
#endif

typedef char int8;
typedef short int int16;
typedef int int32;

#ifdef _MSC_VER
typedef __int64 int64;
#else
typedef long long int int64;
#endif

// length used in all the sizes
typedef uint32 length_t;
typedef uint32 char_t;

}

#endif /* _FALCON_TYPES_H_ */


/* end of types.h */

