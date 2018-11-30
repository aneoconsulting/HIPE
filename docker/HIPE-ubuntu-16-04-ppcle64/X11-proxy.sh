#!/bin/bash

DISPLAY_NUMBER=$1
CONTAINER_DISPLAY=$2

while [ true ]; do
	socat TCP4:localhost:60${DISPLAY_NUMBER} UNIX-LISTEN:display/socket/X${CONTAINER_DISPLAY}
	error=$?
	if [ "$error" != "0" ]; then
		echo "error: for socat $error"
		rm -rf display
	fi
	sleep 1
done
	  
