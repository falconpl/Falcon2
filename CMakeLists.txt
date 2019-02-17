#################################################################
#   FALCON - The Falcon Programming Language
#   FILE: CMakeList
#
#   Toplevel CMAKE list file
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin: Mon, 01 Jan 2018 22:08:54 +0000
#   Touch: Mon, 01 Jan 2018 22:08:54 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
#################################################################

cmake_minimum_required(VERSION 3.1.2)
project(Falcon)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
enable_testing()

##################################################################
# CMAKE bootstrap

include(cmake/utils.cmake)
include(cmake/options.cmake)
include(cmake/version-info.cmake)
include(cmake/system.cmake)

##################################################################
# System specific

include(cmake/test-bigendian.cmake)

##################################################################
# Falcon build environment setup

set(FALCON_TOP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# for install(TARGETS .. ${FALCON_INSTALL_DESTINATIONS})
set(FALCON_INSTALL_DESTINATIONS
  RUNTIME DESTINATION ${FALCON_BIN_DIR}
  LIBRARY DESTINATION ${FALCON_LIB_DIR}
  ARCHIVE DESTINATION ${FALCON_LIB_DIR}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${FALCON_BIN_DIR}" CACHE INTERNAL 
  "Where to put the executables"
)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${FALCON_LIB_DIR}" 
  CACHE INTERNAL "Where to put the libraries"
)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${FALCON_LIB_DIR}" CACHE INTERNAL 
  "Where to put the archives"
)

include_directories( src/include )
include_directories( "${CMAKE_BINARY_DIR}/${FALCON_INC_DIR}" )

#########################################################
# Things to do at main level
#

# Set of OSI/GNU-compliant documentation
include( cmake/install-gnudocs.cmake )


#########################################################
# Subdirectories

add_subdirectory(src)

