##############################################################################
#   FALCON - The Falcon Programming Language
#   FILE: version.cmake
#
#   Install GNU mandatory documentation files
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 02 Jan 2018 16:15:56 +0000
#   Touch : Tue, 02 Jan 2018 18:31:24 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################


set( doc_files
      AUTHORS
      BUILDING
      ChangeLog
      copyright
      LICENSE
      README
      README.md
      TODO
      RELNOTES
   )

install(
   FILES ${doc_files}
   DESTINATION ${FALCON_SHARE_DIR} )

