#################################################################
#
#   FALCON - The Falcon Programming Language
#   FILE: /cmake/install-gnudocs.cmake
#
#   Docs to be installed as part of the GNU-compliancy.
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin: Mon, 01 Jan 2018 22:00:36 +0000
#   Touch: Mon, 01 Jan 2018 22:00:36 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#
#   Released under Apache 2.0 license.
#   
#################################################################

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

