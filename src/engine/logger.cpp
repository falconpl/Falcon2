/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: logger.cpp

  Default Logger Implementation
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Fri, 01 Mar 2019 23:15:12 +0000
  Touch : Sat, 02 Mar 2019 01:33:45 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/logger.h>

namespace Falcon {

thread_local std::string Logger::m_category{""};

}


/* end of logger.cpp */

