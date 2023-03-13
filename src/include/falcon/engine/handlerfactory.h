/*****************************************************************************
  FALCON - The Falcon Programming Language
  FILE: handlerfactory.h

  Collection of statically allocated handlers.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2018 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_HANDLERFACTORY_H_
#define _FALCON_HANDLERFACTORY_H_

#include "falcon/engine/nilhandler.h"
#include "falcon/engine/boolhandler.h"
#include "falcon/engine/inthandler.h"
#include "falcon/engine/floathandler.h"
#include "falcon/engine/stringhandler.h"
#include "falcon/engine/bignumhandler.h"

namespace falcon {

class HandlerFactory {
public:
    static NilHandler nilHandler;
    static BoolHandler boolHandler;
    static IntHandler intHandler;
    static FloatHandler floatHandler;
    static StringHandler stringHandler;
    static BigNumHandler bigNumHandler;
};

}


#endif
