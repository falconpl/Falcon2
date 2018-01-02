#!/bin/bash
##############################################################################
#   FALCON - The Falcon Programming Language
#   FILE: touchdt.sh
#
#   Updates the TOUCH date on boiler plates
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 02 Jan 2018 11:41:45 +0000
#   Touch : Tue, 02 Jan 2018 11:41:45 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

TARGET=$1
if [ -z "$TARGET" ]; then
	echo "Usage: touchdt.sh TARGET"
	exit 1
fi

# Go the the project top directory

pushd `pwd` > /dev/null
SCRIPTPATH="$(cd "$(dirname "$0")"; pwd -P)"
cd "$SCRIPTPATH/.."

if [ ! -e "$TARGET" ]; then
	echo "Target file $TARGET doesn't exist"
	popd > /dev/null
	exit 1
fi

DATE=`date -R`
cat "$TARGET" | \
	sed -e "s/ Touch : [A-Z][a-z].*$/ Touch : $DATE/" \
	> "$TARGET.temp"
mv "$TARGET.temp" "$TARGET"

popd > /dev/null

