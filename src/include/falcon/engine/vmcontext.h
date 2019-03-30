/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: vmcontext.h

  Virtual Machine Context
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 16 Mar 2019 16:41:47 +0000
  Touch : Sat, 16 Mar 2019 16:41:47 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_VMCONTEXT_H_
#define _FALCON_VMCONTEXT_H_

#include <falcon/setup.h>
namespace Falcon {


// TODO
struct Item
{
	int nothing{0};
};

/**
 * The virtual machine context.
 *
 * TODO
 */
class FALCON_API_ VMContext {
public:
	Item& self() noexcept { return m_self; }
	const Item& self() const noexcept {return m_self; }

	int pcount() const {return 0;}

private:
	Item m_self;

};

}

#endif /* _FALCON_VMCONTEXT_H_ */

/* end of vmcontext.h */

