/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: handlerfactory.cpp

  Collection of statically allocated handlers.
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : 
  Touch : 

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/engine/handlerfactory.h>

namespace falcon {

NilHandler HandlerFactory::nilHandler;
BoolHandler HandlerFactory::boolHandler;
IntHandler HandlerFactory::intHandler;
FloatHandler HandlerFactory::floatHandler;
StringHandler HandlerFactory::stringHandler;
BigNumHandler HandlerFactory::bigNumHandler;

}


/* end of code.cpp */

