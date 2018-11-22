#!/bin/bash

cd $(dirname $0)
root_dir=$(pwd -P)
cd install/hipe-core/bin/Debug/

local_dir=$(pwd -P)

export LD_LIBRARY_PATH=${local_dir}/:${local_dir}/../../lib/Debug:${LD_LIBRARY_PATH}



export LD_LIBRARY_PATH=$(readlink -f -n ../../../hipe-modules/lib/Debug/):$(readlink -f -n ../../../hipe-modules/lib/Debug/):${LD_LIBRARY_PATH}
echo "LD_SEARCH = $LD_LIBRARY_PATH"
export PATH=${local_dir}:${PATH}
export PATH=${local_dir}/../../../hipe-modules/lib/Debug/:${PATH}
cd "${local_dir}"

ldd /home/ddubuc/workspace/hipe-aneo/install/hipe-modules/lib/Debug/libfilter.so
#-d -m libfilter.so -s MySharedMemory
killall hipe_engine.bin
killall hipe_server.bin
sleep 2
if [ "$1" == "gdb" ]; then
	gdb -x ${root_dir}/scripts/engine.gdb
elif [ "$1" == "gdbs" ]; then
	
	gdb -x ${root_dir}/scripts/server.gdb

else
	while [ true ]; do
		killall hipe_engine.bin
		killall hipe_server.bin
		hipe_server.bin -m libfilter.so -p 9090
		sleep 1
	done
fi

