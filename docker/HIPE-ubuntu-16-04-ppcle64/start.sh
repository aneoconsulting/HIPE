#!/bin/bash

# Prepare target env
CONTAINER_DISPLAY="10"
CONTAINER_HOSTNAME="lewen12"
CONTAINER_NAME="hipe-ubuntu-16.04-ppc64le"

# Launch the container
NV_GPU=0,2,3 nvidia-docker run -it --rm \
	--hostname ${CONTAINER_HOSTNAME} \
	-p 9000-9008:9000-9008/tcp \
	-p 9090-9140:9090-9140/tcp \
	-p 3100-3199:3100-3199/tcp \
	-p 3000:3000/tcp \
	--cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
	${CONTAINER_NAME} /bin/bash


