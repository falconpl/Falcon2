/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: item.h

  Base Falcon Item
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Tue, 02 Jan 2018 16:38:59 +0000
  Touch : Tue, 02 Jan 2018 18:31:24 +0000

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_ITEM_H_
#define _FALCON_ITEM_H_

#include <falcon/setup.h>
#include <falcon/atomic.h>

namespace Falcon {

class Function;
class Class;

/** Basic item abstraction.*/
class Item
{
	mutable atomic_int m_lock;
	struct
	{
		int32 flags;
		Class *pCls;
		union {
			int32 val32;
			int64 val64;
			numeric number;
			void *pInst;
		}
		data;
		Function* pFunc;
	}
	m_content;

#ifdef _MSC_VER
	#if _MSC_VER < 1299
	#define flagLiteral 0x01
	#define flagIsGarbage 0x02
	#define flagIsOob 0x04
	#define flagLast 0x08
	#define flagContinue 0x10
	#define flagBreak 0x20
	#else
	   static const byte flagLiteral = 0x01;
	   static const byte flagIsGarbage = 0x02;
	   static const byte flagIsOob = 0x04;
	   static const byte flagDoubt = 0x08;
	   static const byte flagContinue = 0x10;
	   static const byte flagBreak = 0x20;
	#endif
#else
   static const byte flagLiteral = 0x01;
   static const byte flagIsGarbage = 0x02;
   static const byte flagIsOob = 0x04;
   static const byte flagDoubt = 0x08;
   static const byte flagContinue = 0x10;
   static const byte flagBreak = 0x20;
#endif
      
   inline Item()
   {
      m_lock = 0;
      m_content.flags = 0;
      m_content.data.val64 = 0;
      m_content.pCls = 0;
      m_content.pFunc = 0;
   }

   inline Item( const Item &other )
   {
	  m_lock = 0;
      other.lock();
      copy( other );
      other.unlock();
   }

   /** Locks this item via atomic CAS loop. */
   void lock() const {
      while(!atomicCAS(m_lock, 0, 1)) {}
   }

   /** Unlock this item via atomic CAS loop. */
   void unlock() const {
      atomicSet(m_lock, 0);
   }


   /** Copies the full contents of another item.
      \param other The copied item.

    This performs a full unlocked flat copy of the original item.
    */
   void copy( const Item &other )
   {
      #ifdef _SPARC32_ITEM_HACK
      register int32 *pthis, *pother;
      pthis = (int32*) this;
      pother = (int32*) &other;
      pthis[0]= pother[0];
      pthis[1]= pother[1];
      pthis[2]= pother[2];
      pthis[3]= pother[3];

      #else
      	  m_content = other.m_content;
      #endif
   }

   /** Assign an item to this item.    
    This operation performs a three-way full interlocked copy of the item.
    To be used when both items might be in a global space.
    */
   void copyInterlocked( const Item& other  )
   {
      Item temp;
      other.lock();
      temp.copy(other);
      other.unlock();

      lock();
      copy(temp);
      unlock();
   }

   /**
    * Assign without interlocking the remote item.
    * \param local The local item from which we do an unlocked copy.
    *
    * This method only locks this item; this is ok
    * if the source is in the local context data stack.
    */
   void copyFromLocal( const Item& local  )
   {
      lock();
      copy(local);
      unlock();
   }

   /**
    * Perform an assignment from a remote locked item to a local copy.
    * \param remote The remote, non local item from which we do locked copy.
    *
    * This method only locks the remote item; this is ok
    * if this item is in the local context data stack.
    */
   void copyFromRemote( const Item& remote )
   {
      remote.lock();
      copy(remote);
      remote.unlock();
   }

};

}


#endif /* _FALCON_ITEM_H_ */

/* end of item.h */
