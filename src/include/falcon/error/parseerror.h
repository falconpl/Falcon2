/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: parseerror.h

  Error raised by the engine when compiling
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 17 Feb 2019 12:58:09 +0000
  Touch : Sun, 17 Feb 2019 14:17:30 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_PARSEERROR_H_
#define _FALCON_PARSEERROR_H_

#include <stdexcept>

namespace Falcon {

class ParseError: public std::runtime_error
{
	//TODO
};

}

#endif /* _FALCON_PARSEERROR_H_ */

/* end of parseerror.h */

