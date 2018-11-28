#!/bin/bash

function usage()
{
	echo "./exec.sh [image_name or image id]"
	exit 0
}

if [ "$#" != "1" ]; then
	usage()
fi


docker exec -e COLUMNS="`tput cols`" -e LINES="`tput lines`" $1
