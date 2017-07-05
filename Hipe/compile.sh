#!/bin/bash

DIR_SCRIPT=$(dirname $0)

cd "${DIR_SCRIPT}"
DIR_SCRIPT=`pwd -P`

export HIPE_EXTERNAL=/mnt/ssd/hipeExternal


mkdir -p build || true

cd build
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_DLIB=ON -DUSE_GPERFTOOLS=OFF -DGPERFTOOLS_DIR=/mnt/ssd/gperftools/install/ ..
make VERBOSE=1 -j8
