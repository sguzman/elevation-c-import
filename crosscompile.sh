#!/bin/bash

set -eux

# Parameters
# $1 = Path to the extracted libxml2, libiconv and zlib directory
# $2 = Build path

LIB_PATH="`readlink -m "$1"`"

if ! test -r $LIB_PATH/bin/libxml2.dll || test "" = "$2"
then
    echo "Usage: $0 library-dir build-dir [--delete]"
fi

if test "x--delete" = "x$3"
then
    rm -rf "$2"
    mkdir "$2"
fi

cd "$2"
cmake -DCMAKE_C_COMPILER=i586-mingw32msvc-gcc \
      -DCMAKE_SYSTEM_NAME=Windows \
      -DLIBXML2DIR=$LIB_PATH \
      $OLDPWD
make -j
