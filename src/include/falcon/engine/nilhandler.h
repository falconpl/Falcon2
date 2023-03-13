/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: nilhandler.h

  Handler for nil items.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/
#ifndef _FALCON_NILHANDLER_H_
#define _FALCON_NILHANDLER_H_

#include "falcon/engine/flathandler.h"

namespace falcon {

struct NilHandler: public FlatHandler {
    virtual ~NilHandler() {}

    virtual ItemData allocate() const {return 0;}

    String typeName() const noexcept override {return "Nil";}
    String toString(ItemData) const noexcept override {return "nil";}
    bool toBool(ItemData) const noexcept override {return false;}
    int64 toInt(ItemData data) const noexcept override {return 0;}
};

}

#endif
