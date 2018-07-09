#!/bin/bash

# Prepare target env
CONTAINER_DISPLAY="10"
CONTAINER_HOSTNAME="hipe_server"

# Create a directory for the socket
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
socat TCP4:localhost:60${DISPLAY_NUMBER} UNIX-LISTEN:display/socket/X${CONTAINER_DISPLAY} &

# Launch the container
docker run -it --rm \
  -e DISPLAY=:${CONTAINER_DISPLAY} \
  -v ${PWD}/display/socket:/tmp/.X11-unix \
  -v ${PWD}/display/Xauthority:/home/hipe-group/.Xauthority \
  --hostname ${CONTAINER_HOSTNAME} \
  -p 9090:9090 -p 9999:9999/udp \
  hipe_server /bin/bash #/home/hipe-group/hipe/install/hipe-core/bin/Release/startHipe.sh
