##############################################################################
#   FALCON - The Falcon Programming Language
#   FILE: src/CMakeLists.txt
#
#   Main core Falcon CMAKE lists
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 09 Jan 2018 14:39:31 +0000
#   Touch : Sun, 17 Feb 2019 14:17:30 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

# Startup
add_subdirectory(include)
add_subdirectory(fut)

# Main Code
add_subdirectory(engine)

# Build completion (tests)
add_subdirectory(test)

# end of CMakeLists.txt

