/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: deephandler.h

  Root handler for the deep items.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_DEEPHANDLER_H_
#define _FALCON_DEEPHANDLER_H_

#include "falcon/engine/handler.h"

namespace falcon {

template<typename T>
struct DeepHandler: public Handler {
    virtual ~DeepHandler() {}

    ItemData allocate() const override {return new T;}
    void destroy(ItemData data) const noexcept override { delete reinterpret_cast<T *>(data.ptrValue);}

    bool isFlat() const noexcept override { return false; }
    bool isCopyFlat() const noexcept override { return false; }
};

}

#endif
