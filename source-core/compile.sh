#!/bin/bash
set -eu

# Get the absolute path to this script with all symlinks resolved.
THIS_SCRIPT="$(readlink -f "$0")"
# Get the directory name of this script.
DIR_SCRIPT="${THIS_SCRIPT%/*}"
# Change into this script's directory.
cd -- "$DIR_SCRIPT"


# Create the build directory and change into it.
mkdir -p build
cd build
mkdir -p ../install

cmake \
	-DCMAKE_BUILD_TYPE=Release \
	-DINSTALL_PREFIX=`pwd`/../install/hipe-core \
	..

make VERBOSE=2 -j1
make VERBOSE=2 install

#make VERBOSE=1 package
