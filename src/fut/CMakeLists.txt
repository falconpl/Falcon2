##############################################################################
#   FALCON - The Falcon Programming Language
#   FILE: src/fut/CMakeLists.txt
#
#   Falcon Unit Test library CMAKE lists
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 09 Jan 2018 14:40:05 +0000
#   Touch : Mon, 15 Jan 2018 00:20:14 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

file(GLOB SOURCES *.cpp)

if(FALCON_SYS_WIN)
   set(SYS_SOURCES
      marktime-win.c++ )
else()
   set(SYS_SOURCES
      marktime-unix.c++ )
endif()
     
add_library(falcon_fut SHARED
   ${SYS_SOURCES}
   ${SOURCES} )

# end of CMakeLists.txt

