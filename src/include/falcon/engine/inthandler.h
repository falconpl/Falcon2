/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: inthandler.h

  Handler for integer items.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/
#ifndef _FALCON_INTHANDLER_H_
#define _FALCON_INTHANDLER_H_

#include "falcon/engine/flathandler.h"
#include <sstream>

namespace falcon {

struct IntHandler: public FlatHandler {
    virtual ~IntHandler() {}

    virtual ItemData allocate() const {return 0LL;}

    String typeName() const noexcept override {return "Int";}
    String toString(ItemData data) const noexcept override {
      std::ostringstream ss;
      ss << data.int64Value;
      return ss.str();
    }

    bool toBool(ItemData data) const noexcept override {return data.int64Value != 0;}
    int64 toInt(ItemData data) const noexcept override {return data.int64Value;}
};

}

#endif
