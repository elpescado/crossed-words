#!/bin/bash

PACKAGE=crossed-words
OS=`uname -s | tr [A-Z] [a-z]`
ARCH=`uname -i`
REVISION=`LANG=C svn info | egrep ^Revision: | cut -d ' ' -f 2`
VERSION="0.$REVISION"

FILE="$PACKAGE-$VERSION-$OS-$ARCH.tar.bz2"

mkdir _build_
cd _build_

mkdir $PACKAGE

mkdir $PACKAGE/pixmaps
cp ../pixmaps/horizontal.png $PACKAGE/pixmaps
cp ../pixmaps/vertical.png   $PACKAGE/pixmaps

cp ../src/crossed-words      $PACKAGE
cp ../src/gaddag.dag         $PACKAGE
cp ../src/dawg.dag           $PACKAGE

mkdir -p $PACKAGE/lang/pl
cp ../src/lang/pl/alphabet.txt $PACKAGE/lang/pl

tar cjf "../$FILE" "$PACKAGE"

cd ..
rm -rf _build_
