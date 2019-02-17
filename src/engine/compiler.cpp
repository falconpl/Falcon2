/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: compiler.cpp

  Falcon2 Source code compiler
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 17 Feb 2019 13:48:59 +0000
  Touch : Sun, 17 Feb 2019 14:17:30 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/


#include <falcon/engine/compiler.h>

namespace Falcon {
Code Compiler::compile(std::istream& input) noexcept
{
	std::string value;
	input >> value;
	return Code(value, 1);
}

}

/* end of compiler.cpp */
