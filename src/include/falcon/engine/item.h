/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: item.h

  Item representing a value in the FPL.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_ITEM_H_
#define _FALCON_ITEM_H_

#include "falcon/types.h"
#include "falcon/engine/handlerfactory.h"

namespace falcon {

class Item {
public:
    ItemData data;
    Handler* handler;

    Item() : data(nullptr), handler(&HandlerFactory::nilHandler) {}

    template<typename T>
    Item(T value) : data(nullptr), handler(nullptr) {
        if constexpr (std::is_same_v<T, bool>) {
            data = ItemData(value);
            handler = &HandlerFactory::boolHandler;
        } else if constexpr (std::is_same_v<T, char>) {
            data = ItemData(value);
            handler = &HandlerFactory::intHandler;
        } else if constexpr (std::is_same_v<T, byte>) {
            data = ItemData(value);
            handler = &HandlerFactory::intHandler;
        } else if constexpr (std::is_same_v<T, int32>) {
            data = ItemData(value);
            handler = &HandlerFactory::intHandler;
        } else if constexpr (std::is_same_v<T, uint32>) {
            data = ItemData(value);
            handler = &HandlerFactory::intHandler;
        } else if constexpr (std::is_same_v<T, int64>) {
            data = ItemData(value);
            handler = &HandlerFactory::intHandler;
        } else if constexpr (std::is_same_v<T, uint64>) {
            data = ItemData(value);
            handler = &HandlerFactory::intHandler;
        } else if constexpr (std::is_same_v<T, numeric>) {
            data = ItemData(value);
            handler = &HandlerFactory::floatHandler;
        } else if constexpr (std::is_same_v<T, ccstring>) {
            data.ptrValue = new String(value);
            handler = &HandlerFactory::stringHandler;
        } else if constexpr (std::is_same_v<T, std::string>) {
            data.ptrValue = new String(value);
            handler = &HandlerFactory::stringHandler;
        }
        else if constexpr (std::is_same_v<T, BigNum>) {
            data.ptrValue = new BigNum(value);
            handler = &HandlerFactory::bigNumHandler;
        } else {
            throw std::invalid_argument("Not a valid item type");
        }
    }

    ~Item() {
        handler->destroy(data);
    }

    template<typename T>
    T get() const {
        if constexpr (std::is_same_v<T, bool>) {
            return data.boolValue;
        } else if constexpr (std::is_same_v<T, int64>) {
            return data.int64Value;
        } else if constexpr (std::is_same_v<T, uint64>) {
           return data.uint64Value;
        } else if constexpr (std::is_same_v<T, numeric>) {
            return data.numericValue;
        } else if constexpr (std::is_same_v<T, String>) {
           return *reinterpret_cast<String *>(data.ptrValue);
        }
        else if constexpr (std::is_same_v<T, BigNum>) {
           return *reinterpret_cast<BigNum *>(data.ptrValue);
        } else {
            throw std::invalid_argument("Not a valid item type");
        }

        return *static_cast<T*>(data);
    }

    String toString() const noexcept {return handler->toString(data);}
    int64 toInt() const noexcept {return handler->toInt(data);}
    bool toBool() const noexcept {return handler->toBool(data);}
};

}

#endif
