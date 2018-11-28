#!/bin/bash
set -eu

# Get the absolute path to this script with all symlinks resolved.
THIS_SCRIPT="$(readlink -f "$0")"
# Get the directory name of this script.
DIR_SCRIPT="${THIS_SCRIPT%/*}"
# Change into this script's directory.
cd -- "$DIR_SCRIPT"

mkdir -p build
cd build
cmake \
	-DCMAKE_BUILD_TYPE=Debug \
	-DWITH_CAFFE=ON \
	-DCMAKE_INSTALL_PREFIX="${DIR_SCRIPT}/../install/hipe-modules" \
	..

NPROC=$(grep -c ^processor /proc/cpuinfo)

make VERBOSE=1 -j${NPROC}
make VERBOSE=1 install
#make VERBOSE=1 package
