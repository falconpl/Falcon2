#!/bin/bash
##############################################################################
#   FALCON - The Falcon Programming Language
#   FILE: bootstrap.sh
#
#   Initialize GIT repository to work in Falcon
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 02 Jan 2018 13:38:35 +0000
#   Touch : Sat, 13 Jan 2018 21:02:34 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

SCRIPTPATH="$(cd "$(dirname "$0")"; pwd -P)"
ln -s ../../githooks/pre-commit "$SCRIPTPATH/../.git/hooks/pre-commit"


