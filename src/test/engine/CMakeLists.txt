##############################################################################
#   FALCON2 - The Falcon Programming Language
#   FILE: src/test/engine/CMakeLists.txt
#
#   CMAKE lists for tests on the Falcon Unit Test subsystem
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 09 Jan 2018 14:46:25 +0000
#   Touch : Sat, 23 Feb 2019 07:15:20 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2019 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

falcon_add_fut( compiler.fut )
falcon_add_fut( singleton.fut )

target_link_libraries(compiler.fut  falcon_engine)
target_link_libraries(singleton.fut  falcon_engine)

# end of CMakeLists.txt

