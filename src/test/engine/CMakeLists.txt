##############################################################################
#   FALCON2 - The Falcon Programming Language
#   FILE: src/test/engine/CMakeLists.txt
#
#   CMAKE lists for tests on the Falcon Unit Test subsystem
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 09 Jan 2018 14:46:25 +0000
#   Touch : Sun, 24 Feb 2019 12:42:56 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2019 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

file(GLOB test_files *.fut.cpp)

foreach(file ${test_files})
	get_filename_component(test_name "${file}" NAME_WE)
	falcon_add_fut(${test_name}.fut)
	target_link_libraries(${test_name}.fut falcon_engine)
endforeach(file)

# end of CMakeLists.txt
