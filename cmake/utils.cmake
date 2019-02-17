##############################################################################
#   FALCON - The Falcon Programming Language
#   FILE: cmake/utils.cmake
#
#   Generic Utilities for the CMAKE build system
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 02 Jan 2018 16:22:30 +0000
#   Touch : Tue, 09 Jan 2018 16:21:23 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

#################################################################
# falcon_add_fut_and_res - Adds a Falcon unit test.
# * source: The source of the unit test
# * other: Additional sources
# * res: Additional resource files to be copied with the test

function( falcon_add_fut_and_res source other res )
   add_executable( "${source}"
      ${source}.cpp
      ${other})

   set_target_properties( "${source}"
         PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${FALCON_BIN_DIR}/tests/")
   
   if(WIN32)
   add_custom_command(TARGET "${source}" POST_BUILD            # Adds a post-build event to MyTest
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different          # which executes "cmake - E copy_if_different..."
        "${CMAKE_BINARY_DIR}/${FALCON_BIN_DIR}/${CMAKE_BUILD_TYPE}/falcon_fut.dll"
        "$<TARGET_FILE_DIR:${source}>")                        # <--this is out-file path
   endif()

   target_link_libraries(${source} falcon_fut)
   add_test("${source}" "${CMAKE_BINARY_DIR}/${FALCON_BIN_DIR}/tests/${source}" )
      
   foreach(resource_file ${res} ) 
      configure_file( "${resource_file}" "${CMAKE_BINARY_DIR}/${FALCON_BIN_DIR}/tests/${resource_file}"
            COPYONLY)
   endforeach()
   
   # We put it in bin/ under build, but in libexec in install
   if(FALCON_INSTALL_TESTS)
        install( ${source}
                 DESTINATION "${FALCON_APP_DIR}/tests/"
                 )
                 
        if("${res}")
            install(
               FILES "${res}"
               DESTINATION "${FALCON_APP_DIR}/tests/"
            )
            
      endif()
   endif()
     
   
endfunction()

#################################################################
# falcon_add_fut_and_res - Adds a Falcon unit test.
# * source: The source of the unit test

function( falcon_add_fut source )
  falcon_add_fut_and_res(${source} "" "")
endfunction()

#################################################################
# Adds a precompiled falcon scripts as a target.
# source - the fal/ftd file that generates the module
#

function( add_fam_target source )
   file( RELATIVE_PATH source_relative "${CMAKE_SOURCE_DIR}" "${source}")
   
   get_filename_component( path_of_fal "${source_relative}"  PATH)
   get_filename_component( name_of_fal "${source_relative}"  NAME_WE)

   # falcon command -- on windows it
   if(UNIX OR APPLE)
      set( falcon_command "${CMAKE_BINARY_DIR}/devtools/icomp.sh" )
   else()
      set( falcon_command "${CMAKE_BINARY_DIR}/devtools/icomp.bat" )
   endif()

   set( output_file "${CMAKE_BINARY_DIR}/${path_of_fal}/${name_of_fal}.fam" )
   set( compile_command ${falcon_command} ${source} ${output_file} )

   add_custom_command(
      OUTPUT "${output_file}"
      COMMAND ${compile_command}
      DEPENDS ${source}
   )
   
   string(REPLACE "/" "_" target_name "${source}" )
         
   add_custom_target(${target_name} ALL DEPENDS "${output_file}" falcon falcon_engine )

   #install must be relative to current source path_of_fal
   file( RELATIVE_PATH single_fal_relative "${CMAKE_CURRENT_SOURCE_DIR}" "${single_fal}")
   get_filename_component( path_of_fal "${single_fal_relative}"  PATH)
   install(FILES "${output_file}" DESTINATION "${FALCON_MOD_DIR}/${path_of_fal}")

endfunction()


#################################################################
# Installs directories containing falcon source files.
# module_dirs - a list of directories conaining fal/ftd
#               source Falcon modules to be installed.
#

function( falcon_install_moddirs module_dirs )

   message( "Installing top modules in ${CMAKE_CURRENT_SOURCE_DIR}" )
   
   foreach(item ${module_dirs} )
      message( "Installing falcon modules in ${item}" )
      file( GLOB_RECURSE files "${item}" "*.fal" "*.ftd" )
      foreach( single_fal ${files} )
         file( RELATIVE_PATH single_fal_relative "${CMAKE_CURRENT_SOURCE_DIR}" "${single_fal}")
         get_filename_component( path_of_fal "${single_fal_relative}"  PATH)

         #Create installation files from in files
         if(NOT FALCON_STRIP_SOURCE_MODS)
            install(
               FILES "${single_fal}"
               DESTINATION "${FALCON_MOD_DIR}/${path_of_fal}"
            )
         endif()
         
         if(FALCON_COMPILE_SOURCE_MODS)
            add_fam_target( ${single_fal} )
         endif()
      endforeach()
   endforeach()
   
endfunction()


#################################################################
# Create a publicly visible option with a default value
# OPT - The option
# desc - The description of the option 
# VAL - ON or OFF

function( set_default_var OPT desc VAL )
	set(NAME "FALCON_${OPT}" )
	if("${${NAME}}" STREQUAL "" )
		set("${NAME}" "${VAL}" PARENT_SCOPE)
		set("${NAME}" "${VAL}" )
	endif()
	Message( STATUS "${NAME}{${${NAME}}} - ${desc}" )
	
	set(lowname "Falcon_${OPT}")
	set("${lowname}" "${VAL}" PARENT_SCOPE)
	#message( STATUS "(lowercase) ${lowname} = ${${lowname}}" )
endfunction()


#################################################################
# Create a publicly visible variable with a default value
# OPT -  The variable name
# desc - The description of the variable
# VAL - the default value for the variable

function( set_default_opt OPT desc VAL )
	option( ${OPT} desc ${VAL} )
	message( STATUS "FALCON_${OPT}{${VAL}} - ${desc} [ON|OFF]" )
endfunction()



#################################################################
# Automatic binding module generation
# module - The name of a falcon module
# lib - the CMAKE name for the bound library 
# desc - description to be sent to message

macro( FalconMakeBinding module lib desc )

   if( FALCON_NATMODS_AUTO )
      find_package( ${lib} )
      if ( ${lib}_FOUND )
         set_default_opt( FALCON_BUILD_${module} "${desc}" ON )
      else()
         set_default_opt( FALCON_BUILD_${module} "${desc}" OFF )
      endif()
   else()
      set_default_opt( FALCON_BUILD_${module} "${desc}" OFF )
   endif()

endmacro()



#################################################################
# Macro for generation of standard falcon modules.
# OPTNAME - the name of the FALCON_BUILD_<option>
# LIBNAME - The name of the module as a library.
# 

macro( FalconDeclareCanonical OPTNAME LIBNAME )
	option( FALCON_BUILD_${OPTNAME} "Build ${OPTNAME} module" OFF)

	if(NOT FALCON_BUILD_${OPTNAME} )
		message( STATUS "NOT building module: ${LIBNAME}")
	return()
	endif()
	message( STATUS "Building module: ${LIBNAME}" )
	set( ${OPTNAME} "${LIBNAME}_fm" )

	project(Falcon_${LIBNAME})
endmacro()

#################################################################
# Declares a feathers module
# LIBNAME - The name of the module as a library.
# SOURCES - The list of source files.
# EXTLIBS - The Extra libraries needed by this module.

macro(FalconDeclareFeather LIBNAME SOURCES EXTLIBS)		
	if( FALCON_STATIC_FEATHERS )
	   foreach(item ${SOURCES} )      	
	   	 LIST(APPEND "${LIBNAME}_SOURCES" "${CMAKE_SOURCE_DIR}/modules/native/feathers/${LIBNAME}/${item}")
	   	 LIST(APPEND FALCON_ENGINE_EXTRA_LINK_LIBS "${EXTLIBS}")
	   endforeach()
	else()
		set( CURRENT_MODULE "${LIBNAME}_fm" )
        set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${FALCON_MOD_DIR}" )
        
		# Builds and links from the source files
		add_library( ${CURRENT_MODULE} MODULE
		   ${SOURCES}
		)
		target_link_libraries( ${CURRENT_MODULE} falcon_engine ${EXTLIBS})

		if(APPLE)
	      set_target_properties(${CURRENT_MODULE} PROPERTIES
	         PREFIX ""
	         SUFFIX ".dylib" )
	   else()
	      set_target_properties(${CURRENT_MODULE} PROPERTIES
	         PREFIX "" )
	   endif()
	
	   install( TARGETS ${CURRENT_MODULE}
	            DESTINATION  "${FALCON_MOD_DIR}" )
		
	   LIST(APPEND Falcon_Feathers_project_targets ${CURRENT_MODULE})
	endif()
		
endmacro()
	
#################################################################
# Declares a canonical module
# LIBNAME - The name of the module as a library.
# SOURCES - The list of source files.
# EXTLIBS - The Extra libraries needed by this module.
# SUBDIR  - Subdirectory under which the canonical module is to be placed.
#

macro(FalconBuildCanonical LIBNAME SOURCES EXTLIBS SUBDIR)		
	if( FALCON_STATIC_MODULES )
	   foreach(item ${SOURCES} )      	
	   	 LIST(APPEND "${LIBNAME}_SOURCES" "${CMAKE_SOURCE_DIR}/modules/native/${LIBNAME}/src/${item}")
	   	 LIST(APPEND FALCON_ENGINE_EXTRA_LINK_LIBS "${EXTLIBS}")
	   endforeach()
	else()
		set( CURRENT_MODULE "${LIBNAME}_fm" )
        set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${FALCON_MOD_DIR}" )
						
		# Builds and links from the source files
		add_library( ${CURRENT_MODULE} MODULE
		   ${SOURCES}
		)
		target_link_libraries( ${CURRENT_MODULE} falcon_engine ${EXTLIBS})

		if(APPLE)
	      set_target_properties(${CURRENT_MODULE} PROPERTIES
	         PREFIX ""
	         SUFFIX ".dylib" )
	    else()
	      set_target_properties(${CURRENT_MODULE} PROPERTIES
	         PREFIX "" )
	    endif()
	   
	    if(SUBDIR)
	   	  set(TARGET_DIR "${FALCON_MOD_DIR}/${SUBDIR}" )
	    else()
	   	  set(TARGET_DIR "${FALCON_MOD_DIR}" )
	    endif()
	
	    install( TARGETS ${CURRENT_MODULE}
	            DESTINATION  "${TARGET_DIR}/" )
	endif()
		
endmacro()

# end of utils.cmake

