##############################################################################
#   FALCON - The Falcon Programming Language
#   FILE: src/test/fut/CMakeLists.txt
#
#   CMAKE lists for tests on the Falcon Unit Test subsystem
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 09 Jan 2018 14:46:25 +0000
#   Touch : Tue, 09 Jan 2018 15:30:48 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

add_executable( fut_smoke
   fut_smoke.cpp)
target_link_libraries(fut_smoke falcon_fut)

# end of CMakeLists.txt

