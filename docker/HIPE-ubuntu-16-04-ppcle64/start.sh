#!/bin/bash

# Prepare target env
CONTAINER_DISPLAY="10"
CONTAINER_HOSTNAME="hipe_server"
CONTAINER_NAME="hipe-ubuntu-16.04-x86_64"

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
#socat TCP4:localhost:60${DISPLAY_NUMBER} UNIX-LISTEN:display/socket/X${CONTAINER_DISPLAY} &
bash X11-proxy.sh ${DISPLAY_NUMBER} ${CONTAINER_DISPLAY} &


# Launch the container
NV_GPU=0 nvidia-docker run -it --rm \
	-e DISPLAY=:${CONTAINER_DISPLAY} \
	-v ${PWD}/display/socket:/tmp/.X11-unix \
	-v ${PWD}/display/Xauthority:/home/hipe-group/.Xauthority \
	--hostname ${CONTAINER_HOSTNAME} \
	--cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
	-p 9000-9800:9000-9800/tcp \
	-p 9000-9800:9000-9800/udp \
	-p 3000-3900:3000-3900/tcp \
	-p 3000-3900:3000-3900/udp \
	${CONTAINER_NAME} /bin/bash 

kill -9 %1
