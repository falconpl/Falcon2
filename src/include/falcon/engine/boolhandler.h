/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: nilhandler.h

  Handler for bool items.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/
#ifndef _FALCON_BOOLHANDLER_H_
#define _FALCON_BOOLHANDLER_H_

#include "falcon/engine/flathandler.h"

namespace falcon {

struct BoolHandler: public FlatHandler {
    virtual ~BoolHandler() {}

    virtual ItemData allocate() const {return false;}

    String typeName() const noexcept {return "Bool";}
    String toString(ItemData data) const noexcept override {return data.boolValue ? "true" : "false";}
    bool toBool(ItemData data) const noexcept override {return data.boolValue;}
    int64 toInt(ItemData data) const noexcept override {return data.boolValue ? 1LL : 0LL;}
};

}

#endif
