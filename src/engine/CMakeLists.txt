##############################################################################
#   FALCON2 - The Falcon Programming Language
#   FILE: src/test/engine/CMakeLists.txt
#
#   CMAKE lists for tests on the Falcon Unit Test subsystem
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 09 Jan 2018 14:46:25 +0000
#   Touch : Sun, 17 Feb 2019 14:17:30 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2019 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

file(GLOB SOURCES *.cpp)
add_definitions(-DFALCON_ENGINE_EXPORTS)
     
# Find Boost.Multiprecision
#set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost REQUIRED) # COMPONENTS multiprecision)


add_library(falcon_engine SHARED
   ${SOURCES} )

# Link against Boost.Multiprecision
target_link_libraries(falcon_engine PUBLIC Boost::headers)

# end of CMakeLists.txt
