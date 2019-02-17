##############################################################################
#   FALCON - The Falcon Programming Language
#   FILE: include/falcon/CMakeLists.txt
#
#   Include directory cmake
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 02 Jan 2018 15:51:57 +0000
#   Touch : Tue, 09 Jan 2018 23:27:02 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

# As the prefix "falcon" is hardcoded in directives #include <falcon/...>
# we want to have include hierarcy as /usr/bin/include/falcon1.0.0/falcon
# while the symlink include/falcon will point not to falcon1.0.0 but to "falcon1.0.0/falcon"
 
set( FALCON_BINARY_INCLUDE_DIR "${PROJECT_BINARY_DIR}/${FALCON_INC_DIR}" )
set( TARGET_DIR "${FALCON_BINARY_INCLUDE_DIR}/falcon" )
configure_file( 
  falcon/config.h.in  "${TARGET_DIR}/config.h"
  @ONLY 
)

file(GLOB falcon_files 
   falcon/*.h
   falcon/fut/*.h)

set(FALCON_HEADERS
   ${falcon_files}
   ${TARGET_DIR}/config.h)

install(FILES ${FALCON_HEADERS} DESTINATION ${FALCON_INC_DIR}/falcon)

#Create a falcon -> falconX.X.X symlink on install.
if(FALCON_SYMLINK_VERSION AND (UNIX OR APPLE) )
 install(CODE "
    EXECUTE_PROCESS(COMMAND ln -sf \"${FALCON_INC_DIR}/falcon\" falcon
       WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
       )
    ")
endif()

set(FALCON_HEADERS ${FALCON_HEADERS} PARENT_SCOPE)
set(FALCON_BINARY_INCLUDE_DIR "${FALCON_BINARY_INCLUDE_DIR}" PARENT_SCOPE)

# end of CMakeLists.txt

