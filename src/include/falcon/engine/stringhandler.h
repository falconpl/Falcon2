/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: stringhandler.h

  Handler for string items.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/
#ifndef _FALCON_STRINGHANDLER_H_
#define _FALCON_STRINGHANDLER_H_

#include "falcon/engine/deepflatcopyhandler.h"


namespace falcon {

struct StringHandler: public DeepFlatCopyHandler<String> {
    virtual ~StringHandler() {}

    String typeName() const noexcept override {return "String";}
    String toString(ItemData data) const noexcept override { return *reinterpret_cast<String *>(data.ptrValue); }

    bool toBool(ItemData data) const noexcept override { return !reinterpret_cast<String *>(data.ptrValue)->empty(); }
    int64 toInt(ItemData data) const noexcept override { return std::stoll(*reinterpret_cast<String *>(data.ptrValue)); }
};

}

#endif
