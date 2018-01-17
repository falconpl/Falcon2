#!/bin/bash
##############################################################################
#   FALCON - The Falcon Programming Language
#   FILE: mktp.sh
#
#   Configure file templates
#   -------------------------------------------------------------------
#   Author: Giancarlo Niccolai
#   Begin : Tue, 02 Jan 2018 11:41:45 +0000
#   Touch : Wed, 17 Jan 2018 20:53:39 +0000
#
#   -------------------------------------------------------------------
#   (C) Copyright 2018 The Falcon Programming Language
#   Released under Apache 2.0 License.
##############################################################################

TARGET=$1
DESC=$2
if [ -z "$TARGET" -o -z "$DESC" ]; then
	echo "Usage: mktp.sh TARGET DESCRIPTION"
	exit 1
fi

# Go the the project top directory

pushd `pwd` > /dev/null
SCRIPTPATH="$(cd "$(dirname "$0")"; pwd -P)"
cd "$SCRIPTPATH/.."

if [ -e "$TARGET" ]; then
	echo "Target file $TARGET already exists"
	popd > /dev/null
	exit 1
fi

AUTHOR=`git config --list |grep user.name |cut -d = -f 2`
NOW=`date -R`
YEAR=`date +%Y`
TGNAME=`basename "$TARGET"`
TGFILE=$(echo $TGNAME | cut -d . -f 1)
EXT="${TGNAME##*.}"
UPPER_TGNAME=$(echo $TGFILE|tr '[:lower:]' '[:upper:]')
DEFINE_NAME="_FALCON_${UPPER_TGNAME}_H_"

case $EXT in
	"cpp")
		SOURCE="templates/template.cpp"
		;;	
	"h")
		SOURCE="templates/template.h"
		;;
	*)
		SOURCE="templates/template.script"
		;;
esac

echo "Configuring $TARGET"
SEDTARGET=$(echo $TARGET| sed -e "s/\\//\\\\\\//g")
cat $SOURCE | \
	sed -e "s/@name@/$TGNAME/" -e "s/@short@/$DESC/" -e "s/@begin@/$NOW/" \
		-e "s/@author@/$AUTHOR/" -e "s/@DEFINE_NAME@/$DEFINE_NAME/" \
		-e "s/@year@/$YEAR/" -e "s/@path@/$SEDTARGET/"\
	> "$TARGET"
popd > /dev/null

