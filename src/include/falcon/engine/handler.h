/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: handler.h

  Item handler; this represent the item type in the language.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_HANDLER_H_
#define _FALCON_HANDLER_H_

#include "falcon/types.h"
#include "falcon/engine/itemdata.h"

namespace falcon {

class Item;

struct Handler {
    virtual ~Handler() {}
    
    virtual bool isFlat() const noexcept = 0;
    virtual bool isCopyFlat() const noexcept = 0;

    virtual ItemData allocate() const =0;
    virtual void destroy(ItemData data) const noexcept =0;

    virtual String typeName() const noexcept = 0;
    virtual String toString(ItemData data) const noexcept = 0;
    virtual bool toBool(ItemData data) const noexcept = 0;
    virtual int64 toInt(ItemData data) const noexcept = 0;
};

}

#endif
