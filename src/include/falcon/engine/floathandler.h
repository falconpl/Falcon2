/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: floathandler.h

  Handler for float items.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/
#ifndef _FALCON_FLOATHANDLER_H_
#define _FALCON_FLOATHANDLER_H_

#include "falcon/engine/flathandler.h"
#include <sstream>

namespace falcon {

struct FloatHandler: public FlatHandler {
    virtual ~FloatHandler() {}

    virtual ItemData allocate() const {return 0.0;}

    String typeName() const noexcept override {return "Float";}
    String toString(ItemData data) const noexcept override {
      std::ostringstream ss;
      ss << data.numericValue;
      return ss.str();
    }

    bool toBool(ItemData data) const noexcept override {return data.numericValue != 0.0;}
    int64 toInt(ItemData data) const noexcept override {return static_cast<int64>(data.numericValue);}
};

}

#endif
