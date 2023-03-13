/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: flathandler.h

  Root handler for the flat items.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/
#ifndef _FALCON_FLATHANDLER_H_
#define _FALCON_FLATHANDLER_H_

#include "falcon/engine/handler.h"

namespace falcon {

struct FlatHandler: public Handler {
    virtual ~FlatHandler() {}

    ItemData allocate() const override { return ItemData(false); }
    void destroy(ItemData) const noexcept override {}
    
    bool isFlat() const noexcept override{ return true; }
    bool isCopyFlat() const noexcept override{ return true; }
    
};

}

#endif
