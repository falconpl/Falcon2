/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: atomic_gcc.h

  Atomic operations (GCC)
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 02 Jan 2018 16:41:16 +0000
  Touch : Tue, 02 Jan 2018 18:31:24 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_ATOMIC_GCC_H_
#define _FALCON_ATOMIC_GCC_H_

#include <falcon/types.h>

namespace Falcon {
/**  An alias to the atomic integer type.
 */
typedef volatile int32 atomic_int;
typedef int32 atomic_int_base;

/** Performs an atomic thread safe increment. */
inline int32 atomicInc( atomic_int& atomic )
{
   return __sync_add_and_fetch( &atomic, 1);
}

/** Performs an atomic thread safe decrement. */
inline int32 atomicDec( atomic_int& atomic )
{
   return __sync_add_and_fetch( &atomic, -1);
}

/** Performs an atomic thread safe addition. */
inline int32 atomicAdd( atomic_int& atomic, atomic_int value )
{
   return __sync_add_and_fetch( &atomic, value );
}

/** Perform a threadsafe fetch */
inline int32 atomicFetch( const atomic_int& atomic ) {
   __sync_synchronize ();
   return atomic;
}

/** Perform a threadsafe set.*/
inline void atomicSet( atomic_int& atomic, atomic_int value ) {
   __sync_lock_test_and_set( &atomic, value );
}

/** Sets the given value in atomic, and returns the previous value. */
inline atomic_int_base atomicExchange( atomic_int& atomic, atomic_int value )
{
   return (int32) __sync_lock_test_and_set( &atomic, value );
}

inline bool atomicCAS( atomic_int& target, atomic_int compareTo, atomic_int newVal )
{
   return __sync_bool_compare_and_swap( &target, compareTo, newVal );
}

inline atomic_int_base atomicXor( atomic_int& target, atomic_int value )
{
   return __sync_fetch_and_xor(&target, value);
}

inline atomic_int_base atomicAnd( atomic_int& target, atomic_int value )
{
   return __sync_fetch_and_and(&target, value);
}

inline atomic_int_base atomicOr( atomic_int& target, atomic_int value )
{
   return __sync_fetch_and_or(&target, value);
}

}

#endif

/* end of atomic_gcc.h */
