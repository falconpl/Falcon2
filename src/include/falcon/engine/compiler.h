/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: compiler.h

  Compiler for Falcon2 Source
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 17 Feb 2019 12:44:10 +0000
  Touch : Sun, 17 Feb 2019 14:17:30 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_COMPILER_H_
#define _FALCON_COMPILER_H_

#include <istream>
#include "falcon/engine/code.h"

namespace falcon {
class Compiler {

public:
	/**
	 * Compiles a Falcon2 Source from a given input stream.
	 * @param input The input stream
	 * @return a Code instance, returned by move semantic.
	 * @throw CompilationError on error.
	 */

	Code compile(std::istream& input) noexcept;
};
}

#endif /* _FALCON_COMPILER_H_ */

/* end of compiler.h */

