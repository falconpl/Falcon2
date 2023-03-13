/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: atomic_vc.h

  Atomic operations (Visual C++)
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 02 Jan 2018 16:41:16 +0000
  Touch : Tue, 02 Jan 2018 18:31:24 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_ATOMIC_GCC_H_

#ifndef _FALCON_ATOMIC_VC_H_
#define _FALCON_ATOMIC_VC_H_
#include <falcon/types.h>

#include <windows.h>
#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace falcon {
/**  An alias to the atomic integer type.
 */
typedef LONG atomic_int;

/** Performs an atomic thread safe increment. */
inline int32 atomicInc( atomic_int& atomic )
{
   return InterlockedIncrement( &atomic );
}

/** Performs an atomic thread safe decrement. */
inline int32 atomicDec( atomic_int& atomic )
{
   return InterlockedDecrement( &atomic );
}

/** Performs an atomic thread safe addition. */
inline int32 atomicAdd( atomic_int& atomic, atomic_int value )
{
#ifdef _M_IX86
   return InterlockedExchangeAdd( &atomic, value );
#else
   return InterlockedAdd( &atomic, value );
#endif
}

/** Perform a threadsafe fetch */
inline int32 atomicFetch( const atomic_int& atomic ) {
   return InterlockedCompareExchange( (volatile LONG*)&atomic, 0, 0 );
}

/** Perform a threadsafe set.*/
inline void atomicSet( atomic_int& atomic, atomic_int value ) {
   InterlockedExchange( &atomic, value );
}

/** Sets the given value in atomic, and returns the previous value. */
inline atomic_int atomicExchange( atomic_int& atomic, atomic_int value )
{
   return InterlockedExchange( &atomic, value );
}

inline bool atomicCAS( atomic_int& target, atomic_int compareTo, atomic_int newVal )
{
   return InterlockedCompareExchange( &target, newVal, compareTo ) == compareTo;
}

inline atomic_int atomicXor( atomic_int& target, atomic_int value )
{
#ifdef _M_IX86
   return _InterlockedXor(&target, value);
#else
   return InterlockedXor(&target, value);
#endif
}

inline atomic_int atomicAnd( atomic_int& target, atomic_int value )
{
#ifdef _M_IX86
   return _InterlockedAnd( &target, value );
#else
   return InterlockedAnd(&target, value);
#endif
}

inline atomic_int atomicOr( atomic_int& target, atomic_int value )
{
#ifdef _M_IX86
   return _InterlockedOr(&target, value);
#else
   return InterlockedOr(&target, value);
#endif
}

}

#endif

/* end of atomic_vc.h */
