#!/bin/bash

self_="$(readlink -f "$0")"
script_dir="${self_%/*}"
RUN_DRY=0

function RUN()
{
	echo "$@"
	if [ "${RUN_DRY}" != "1" ]; then
		"$@"
		err=$?
		if [ "$err" != "0" ]; then
			echo "error: $@"
			echo "error: the command $1 return error code $err"
			exit 1
		else
			echo "Command return Sucess ($err)"
		fi
	fi
}

RUN export USER_NAME="${USER}"
RUN export USER_HOME="${script_dir}/host/"
RUN mkdir -p "${USER_HOME}"

RUN export HIPE_EXTERNAL=~/workspace/HIPE_EXTERNAL-V2
RUN mkdir -p ${HIPE_EXTERNAL}

# Enable Ubuntu multiverse packages. This also enables the matching deb-src
# repositories.
RUN sudo sed -i 's@^#* *\(deb.* multiverse\)$@\1@' /etc/apt/sources.list

# Update system and package lists.
#RUN sudo apt-get update

# Install compatible system libraries to avoid compiling them in Hipe External.
RUN sudo apt-get install -y --no-install-recommends \
  python2.7 \
  python2.7-dev \
  liblivemedia-dev \
  libgtk2.0 \
  libgtk2.0-dev \
  pkg-config \
  xauth \
  x11-apps \
  nano curl \
  sudo \
  autoconf \
  automake \
  bsdtar \
  build-essential \
  ca-certificates ssl-cert \
  gcc \
  gcc-5 \
  git \
  libtool \
  python3 \
  python3-yaml \
  wget \
  yasm \
  make \
  libcurl4-openssl-dev \
  libssl-dev \
  libx11-dev \
  python3-dev \
  python-dev \
  python-numpy \
  python3-numpy \
  python-setuptools \
  python3-pip \
  python-pip \
	libgstreamer1.0-0 \
	libgstreamer1.0-dev \
	gstreamer1.0-plugins-base \
	gstreamer1.0-plugins-base-apps \
	libges-1.0-0 \
	gstreamer-tools \
	libges-1.0-dev \
	libgstreamer-plugins-base1.0-0 \
	libgstreamer-plugins-base1.0-dev \
	libgstreamer-plugins-good1.0-0 \
	libgstreamer-plugins-good1.0-dev \
	libgstreamer-plugins-bad1.0-dev \
	libgstreamer-plugins-bad1.0-0 \
	gstreamer1.0-plugins-good \
	gstreamer1.0-plugins-bad \
	gstreamer1.0-plugins-ugly \
	gstreamer1.0-vaapi \
	gstreamer1.0-libav \
	libavresample-dev \
	v4l-utils \
	libv4l-dev \
  libgoogle-glog-dev \
  libgflags-dev \
  libprotobuf-dev \
  libprotoc-dev \
  libtesseract-dev \
  libleptonica-dev \
  liblept5 \
  libeigen3-dev \
  liblapack-dev \
  liblapacke-dev \
  libopenblas-base \
  libopenblas-dev \
  libatlas-dev \
  libfreetype6-dev \
  libtheora-dev \
  libvorbis-dev \
  libavutil-dev \
  libavcodec-dev \
  libswscale-dev \
  libavformat-dev \
  libavutil-dev \
	libicu55 \
	libicu-dev \
	libhdf5-dev \
	protobuf-compiler \
	liblmdb-dev \
	libleveldb-dev \
	libsnappy-dev \
	libturbojpeg




RUN sudo python2.7 -m pip install numpy
RUN sudo python3   -m pip install numpy


# Install the build deps for the compilation targets.
#apt-get build-dep \
#  opencv \
#  ffmpeg \

# Copy the local build scripts and source files. The source files are downloaded
# before building the image to avoid redundant downloads when rebuilding the
# image. The entire hipe directory is not copied here because any changes to
# the source files in hipe/${USER} would invalidate the cache and trigger a rerun
# off all commands from this point. This must be avoided because the following
# compilation steps take a long time to complete.
#COPY hipe/build /home/hipe/build
#COPY hipe/hipe_external ${HIPE_EXTERNAL}
#mkdir -p ${HIPE_EXTERNAL}/build

# Download before building the image to avoid redownloading the files with
# every build.
# TODO: Maybe make this optional. Given that the Git repo currently prompts for
#       a name and password, all downloads cannot yet be automated anyway.
#cd ${HIPE_EXTERNAL}/build
#python3 prepare_download.py boost opencv dlib
#bash download_sources.sh





###########
#DDU
##########

#####
# Create download area
####
RUN mkdir -p ${USER_HOME}/downloads
RUN cd ${USER_HOME}/downloads

if [ ! -f cmake-3.11.4-Linux-x86_64.sh ]; then
	RUN wget https://cmake.org/files/v3.11/cmake-3.11.4-Linux-x86_64.sh
fi



#####
# Create tools folder
#####

RUN mkdir -p ${USER_HOME}/utils/
RUN cd ${USER_HOME}/utils
RUN bash ${USER_HOME}/downloads/cmake-3.11.4-Linux-x86_64.sh --skip-license

RUN export PATH="${USER_HOME}/utils:${USER_HOME}/utils/bin:${PATH}"

RUN mkdir -p ${HIPE_EXTERNAL}/linux64
RUN mkdir -p ${HIPE_EXTERNAL}/downloads
RUN mkdir -p ${HIPE_EXTERNAL}/build

######
#Download all requierements
######


RUN mkdir -p ${HIPE_EXTERNAL}/downloads
RUN cd ${HIPE_EXTERNAL}/downloads

# cuda
if [ ! -f cuda_10.0.run ]; then
	RUN wget -c -O cuda_10.0.run https://developer.nvidia.com/compute/cuda/10.0/Prod/local_installers/cuda_10.0.130_410.48_linux
fi
# Intel_TBB: intel-tbb-2018_U2.tar.gz
if [ ! -f intel-tbb-2018_U2.tar.gz ]; then
	RUN wget -c -O intel-tbb-2018_U2.tar.gz https://github.com/01org/tbb/archive/2018_U2.tar.gz
fi
# OpenCV: opencv-3.4.0.zip
if [ ! -f opencv-3.4.2.zip ]; then
	RUN wget -c -O opencv-3.4.2.zip https://github.com/opencv/opencv/archive/3.4.2.zip
fi
# OpenCV: opencv_contrib-3.4.0.zip
if [ ! -f opencv_contrib-3.4.2.zip ]; then
	RUN wget -c -O opencv_contrib-3.4.2.zip https://github.com/opencv/opencv_contrib/archive/3.4.2.zip
fi
	# Dlib: dlib-19.16.zip
if [ ! -f dlib-19.16.zip ]; then
	RUN wget -c -O dlib-19.16.zip http://dlib.net/files/dlib-19.16.zip
fi
# Boost: boost_1_62_0.tar.gz
if [ ! -f boost_1_66_0.tar.gz ]; then
	RUN wget -c -O boost_1_66_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.66.0/boost_1_66_0.tar.gz
fi





######
#Build dependencies
######

RUN cd ${script_dir}/utils

RUN bash build.sh -a "${HIPE_EXTERNAL}/downloads" \
	-b "${HIPE_EXTERNAL}/build" \
	-i "${HIPE_EXTERNAL}/linux64" \
	-g "${HIPE_EXTERNAL}/downloads" -d -c glog -n 8

RUN bash build.sh -a "${HIPE_EXTERNAL}/downloads" \
	-b "${HIPE_EXTERNAL}/build" \
	-i "${HIPE_EXTERNAL}/linux64" \
	-g "${HIPE_EXTERNAL}/downloads" -d -c gflags -n 8

RUN bash build.sh -a "${HIPE_EXTERNAL}/downloads" \
	-b "${HIPE_EXTERNAL}/build" \
	-i "${HIPE_EXTERNAL}/linux64" \
	-g "${HIPE_EXTERNAL}/downloads" -d -c boost_d -n 8
	
RUN bash build.sh -a "${HIPE_EXTERNAL}/downloads" \
	-b "${HIPE_EXTERNAL}/build" \
	-i "${HIPE_EXTERNAL}/linux64" \
	-g "${HIPE_EXTERNAL}/downloads" -d -c dlib_d -n 8

RUN bash build.sh -a "${HIPE_EXTERNAL}/downloads" \
	-b "${HIPE_EXTERNAL}/build" \
	-i "${HIPE_EXTERNAL}/linux64" \
	-g "${HIPE_EXTERNAL}/downloads" -d -c opencv_d -n 8


RUN bash build.sh -a "${HIPE_EXTERNAL}/downloads" \
	-b "${HIPE_EXTERNAL}/build" \
	-i "${HIPE_EXTERNAL}/linux64" \
	-g "${HIPE_EXTERNAL}/downloads" -d -c borringssl -n 8


if [ "$(id -u ddubuc 2>&1 | grep -v 'id:')" == "" ]; then
	##Now everything is built then add user ${USER} and change all rights for him
RUN 	sudo useradd -d ${USER_HOME} -ms /bin/bash -g root -G sudo -p ${USER} ${USER}
RUN 	sudo groupadd ${USER}
RUN 	sudo echo "${USER}:${USER}" | chpasswd && adduser ${USER} sudo
RUN 	sudo echo "${USER} ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers
RUN 	cd ${USER_HOME}
RUN 	sudo chown -hR ${USER}:${USER} *
RUN 	sudo chown -hR ${USER}:${USER} ${USER_HOME}
fi

RUN cp ${script_dir}/mysql/install-webserver.sh ${USER_HOME}/install-webserver.sh
RUN rm -rf ${USER_HOME}/mysql_db
RUN mkdir -p ${USER_HOME}/mysql_db
RUN cp -f ${script_dir}/mysql/hipe.sql ${USER_HOME}/mysql_db/
RUN cd ${USER_HOME}/

RUN chmod +x ${USER_HOME}/install-webserver.sh
RUN sudo bash ${USER_HOME}/install-webserver.sh ${USER_HOME}/mysql_db/ ${USER_NAME}

if [ ! -f /usr/local/bin/node ]; then
	RUN sudo ln -s /usr/bin/nodejs /usr/local/bin/node
fi

if [ ! -f /usr/local/bin/npm ]; then
	RUN sudo ln -s /usr/bin/npm /usr/local/bin/npm
fi


RUN cp ${script_dir}/mysql/webserver-hipe.tar.gz ${USER_HOME}/webserver-hipe.tar.gz
RUN cd ${USER_HOME}/
RUN tar xvzf webserver-hipe.tar.gz
RUN cd ${USER_HOME}/webserver-hipe

RUN npm install

RUN export PATH="${USER_HOME}/webserver-hipe/node_modules/.bin:${PATH}"
RUN gulp js
RUN gulp css
RUN gulp sass
RUN gulp fonts
RUN gulp html

#run node app.js

#echo "echo 'start database for webserver'; sudo sudo service mysql restart; sleep 2; cd ${USER_HOME}/webserver-hipe/; node app.js &" >> ${USER_HOME}/.bashrc
#echo "sleep 2; echo 'Webserver is started'" >> ${USER_HOME}/.bashrc
#echo "echo ''" >> ${USER_HOME}/.bashrc
#echo "echo '#################################################################'" >> ${USER_HOME}/.bashrc
#echo "echo ''" >> ${USER_HOME}/.bashrc
#echo "echo 'HELLO : To Start HIPE run this script : '" >> ${USER_HOME}/.bashrc
#echo "echo 'bash ${USER_HOME}/hipe/install/hipe-core/bin/Release/startHipe.sh'" >> ${USER_HOME}/.bashrc
#echo "echo ''" >> ${USER_HOME}/.bashrc
#echo "echo '#################################################################'" >> ${USER_HOME}/.bashrc
#echo "echo ''" >> ${USER_HOME}/.bashrc

# Build Hipe.
#RUN cd /home/${USER}
#RUN git clone https://github.com/aneoconsulting/HIPE.git hipe
#RUN cd /home/${USER}/hipe

#RUN bash compile-ALL.sh


