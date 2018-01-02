##############################################################################
#   FALCON - The Falcon Programming Language
#   FILE: test-bigendian.cmake
#
#   Check system endianity
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 02 Jan 2018 16:15:56 +0000
#   Touch : Tue, 02 Jan 2018 18:31:24 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

include(TestBigEndian)

TEST_BIG_ENDIAN(falcon_big_endian)
if(falcon_big_endian)
   set(FALCON_LITTLE_ENDIAN 0) 
   message(STATUS "Endianity on \"${CMAKE_SYSTEM}\" as BIG ENDIAN" )
else(falcon_big_endian)
   set(FALCON_LITTLE_ENDIAN 1)
   message(STATUS "Endianity on \"${CMAKE_SYSTEM}\" as LITTLE ENDIAN" )
endif(falcon_big_endian)

