/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: itemdata.h

  Raw type used as the data component of items.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_ITEMDATA_H_
#define _FALCON_ITEMDATA_H_

#include "falcon/types.h"

namespace falcon {

union ItemData {
    bool boolValue;
    int64 int64Value;
    uint64 uint64Value;
    numeric numericValue;
    void* ptrValue;

    template<typename T>
    ItemData(const T& value) {
        if constexpr (std::is_same_v<T, bool>) {
            boolValue = value;
        } else if constexpr (std::is_same_v<T, char>) {
            uint64Value = static_cast<uint64>(value);
        } else if constexpr (std::is_same_v<T, byte>) {
            int64Value = static_cast<int64>(value);
        } else if constexpr (std::is_same_v<T, int32>) {
            int64Value = static_cast<int64>(value);
        } else if constexpr (std::is_same_v<T, int64>) {
            int64Value = value;
        } else if constexpr (std::is_same_v<T, uint32>) {
            uint64Value = static_cast<uint64>(value);
        } else if constexpr (std::is_same_v<T, uint64>) {
            uint64Value = value;
        } else if constexpr (std::is_same_v<T, numeric>) {
            numericValue = value;
        } else {
            ptrValue = static_cast<void*>(const_cast<T*>(&value));
        }
    }
};

}

#endif
