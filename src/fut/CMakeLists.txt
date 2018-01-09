##############################################################################
#   FALCON - The Falcon Programming Language
#   FILE: src/fut/CMakeLists.txt
#
#   Falcon Unit Test library CMAKE lists
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 09 Jan 2018 14:40:05 +0000
#   Touch : Tue, 09 Jan 2018 15:30:48 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

file(GLOB SOURCES *.cpp)

add_library(falcon_fut SHARED
   ${SOURCES} )

# end of CMakeLists.txt

