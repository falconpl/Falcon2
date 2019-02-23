/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: item.h

  Falcon Item
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 23 Feb 2019 04:28:03 +0000
  Touch : Sat, 23 Feb 2019 07:17:16 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_ITEM_H_
#define _FALCON_ITEM_H_

namespace Falcon {

class IHandler;

class Item {
public:
	Item() {
		setNil();
	}

	Item(const Item&) = default;
	Item(Item&&) = default;
	~Item() = default;

public:

};
}

#endif /* _FALCON_ITEM_H_ */

/* end of item.h */

