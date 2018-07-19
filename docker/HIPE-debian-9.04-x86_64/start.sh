#!/bin/bash

# Prepare target env
CONTAINER_DISPLAY="10"
CONTAINER_HOSTNAME="hipe_server"
CONTAINER_NAME="hipe-debian-9.4-x86_64"

# Create a directory for the socket
rm -rf display
mkdir -p display/socket
touch display/Xauthority

# Get the DISPLAY slot
DISPLAY_NUMBER=$(echo $DISPLAY | cut -d. -f1 | cut -d: -f2)

# Extract current authentication cookie
AUTH_COOKIE=$(xauth list | grep "^$(hostname)/unix:${DISPLAY_NUMBER} " | awk '{print $3}')
echo $AUTH_COOKIE

# Create the new X Authority file
xauth -f display/Xauthority add ${CONTAINER_HOSTNAME}/unix:${CONTAINER_DISPLAY} MIT-MAGIC-COOKIE-1 ${AUTH_COOKIE}

# Proxy with the :0 DISPLAY
bash X11-proxy.sh ${DISPLAY_NUMBER} ${CONTAINER_DISPLAY} &

echo ""
echo "#################################################################"
echo ""
echo "HELLO : To Start HIPE run this script : "
echo "bash /home/hipe-group/hipe/install/hipe-core/bin/Release/startHipe.sh"
echo ""
echo "#################################################################"
echo ""

if [ "$1" == "run" ]; then
   arg="/home/hipe-group/hipe/install/hipe-core/bin/Release/startHipe.sh"
else
	arg=""
fi

if [ "$1" == "dev" ]; then
	arg=""
	dockerOpt="--mount type=bind,source=$(pwd)/../../,target=/home/hipe-group/hipe"
else
	dockerOpt=""
fi


# Launch the container

#--mount type=bind,source="$(pwd)"/hipe,target=/home/hipe-group/hipe \
docker run -it --rm \
	   -e DISPLAY=:${CONTAINER_DISPLAY} \
	   -v ${PWD}/display/socket:/tmp/.X11-unix \
	   -v ${PWD}/display/Xauthority:/home/hipe-group/.Xauthority \
	   --hostname ${CONTAINER_HOSTNAME} \
	   ${dockerOpt} \
	   -p 9090:9090 -p 9999:9999/udp \
	   -p 3000:3000 \
	   ${CONTAINER_NAME} /bin/bash ${arg}


#kill -9 %1
