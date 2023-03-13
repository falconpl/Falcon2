/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: bignumhandler.h

  Handler for arbitrary precision number items.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/
#ifndef _FALCON_BIGNUMHANDLER_H_
#define _FALCON_BIGNUMHANDLER_H_

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <sstream>
#include "falcon/engine/deepflatcopyhandler.h"


namespace falcon {

using BigNum = boost::multiprecision::cpp_dec_float_50;

struct BigNumHandler: public DeepFlatCopyHandler<BigNum> {
  virtual ~BigNumHandler() {}

  String typeName() const noexcept override {return "BigNum";}
  String toString(ItemData data) const noexcept override { 
    std::ostringstream ss;
    ss << *reinterpret_cast<BigNum *>(data.ptrValue);
    return ss.str();
  }

  bool toBool(ItemData data) const noexcept override { return ! static_cast<BigNum *>(data.ptrValue)->is_zero(); }
  int64 toInt(ItemData data) const noexcept override { return reinterpret_cast<BigNum *>(data.ptrValue)->convert_to<int64>(); }
};

}

#endif
