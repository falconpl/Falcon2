/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: deepflatcopyhandler.h

  Root handler for the deep items with a flat copy behavior.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_DEEPFLATCOPYHANDLER_H_
#define _FALCON_DEEPFLATCOPYHANDLER_H_

#include "falcon/engine/deephandler.h"

namespace falcon {

template<typename T>
struct DeepFlatCopyHandler: public DeepHandler<T> {
    bool isCopyFlat() const noexcept override { return true; }
};

}

#endif
