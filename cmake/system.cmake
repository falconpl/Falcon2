##############################################################################
#   FALCON - The Falcon Programming Language
#   FILE: cmake/system.cmake
#
#   System-specific settings
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Sun, 14 Jan 2018 22:52:01 +0000
#   Touch : Mon, 15 Jan 2018 00:20:14 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

##############################################################################
# System family detection
##############################################################################

if(WIN32)
   SET( FALCON_HOST_SYSTEM "WINDOWS" )
   SET( FALCON_SYSTEM_WIN 1 )
else()
   if(APPLE)
      set( FALCON_HOST_SYSTEM "MAC" )
      set( FALCON_SYSTEM_MAC 1 )
   elseif(UNIX)
      set( FALCON_HOST_SYSTEM "UNIX" )
      SET( FALCON_SYSTEM_UNIX 1 )
   else()
      message(FATAL_ERROR "Sorry, can't determine system type" )
   endif()
endif()

##############################################################################
#  Endianity detection
##############################################################################
include(TestBigEndian)

message(STATUS "Testing endianity on ${CMAKE_SYSTEM}" )
TEST_BIG_ENDIAN(falcon_big_endian)
if(falcon_big_endian)
   set(FALCON_LITTLE_ENDIAN 0)
else(falcon_big_endian)
   set(FALCON_LITTLE_ENDIAN 1)
endif(falcon_big_endian)

