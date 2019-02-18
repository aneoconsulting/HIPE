#!/bin/bash
shopt -s expand_aliases

RUN_STEP=0

export USER_NAME="${USER}"
export USER_HOME="/home/${USER}/hipe-project"
mkdir -p "${USER_HOME}"
export HIPE_EXTERNAL=${USER_HOME}/HIPE_EXTERNAL
mkdir -p ${HIPE_EXTERNAL}


if [ $# -ge 1 ]; then
	RUN_STEP=$1
fi

self_="$(readlink -f "$0")"
script_dir="${self_%/*}"
RUN_DRY=0


current_count=0

function RUN()
{
	if [ "${RUN_DRY}" != "1" ]; then
		if [ ${current_count} -ge ${RUN_STEP} ]; then
			echo "RUN STEP : ${current_count} : $@"
			"$@"
		else
			echo "SKIP STEP ${current_count} : $@"
		fi
		
		current_count=$((current_count + 1))
		
		err=$?
		if [ "$err" != "0" ]; then
			echo "error: $@"
			echo "error: the command $1 return error code $err"
			exit 1
		else
			echo "Command return Sucess ($err)"
		fi
	else
		echo "$@"
	fi
}

alias WORKDIR=cd

alias ENV=export

function COPY()
{
	RUN cp $@
}



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
	gstreamer1.0-tools \
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
  libtesseract-dev \
  libleptonica-dev \
  liblept5 \
  libeigen3-dev \
  liblapack-dev \
  liblapacke-dev \
  libopenblas-base \
  libopenblas-dev \
  libatlas-base-dev \
  libfreetype6-dev \
  libtheora-dev \
  libvorbis-dev \
  libavutil-dev \
  libavcodec-dev \
  libswscale-dev \
  libavformat-dev \
  libavutil-dev \
	libicu60 \
	libicu-dev \
	libhdf5-dev \
	protobuf-compiler \
	liblmdb-dev \
	libleveldb-dev \
	libsnappy-dev \
	libturbojpeg \
  libgoogle-glog-dev \
  libgflags-dev \
  golang-go




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
WORKDIR ${USER_HOME}/downloads

if [ ! -f cmake-3.13.2-Linux-x86_64.sh ]; then
	RUN wget https://github.com/Kitware/CMake/releases/download/v3.13.2/cmake-3.13.2-Linux-x86_64.sh
fi



#####
# Create tools folder
#####

RUN mkdir -p ${USER_HOME}/utils/
WORKDIR ${USER_HOME}/utils
RUN bash ${USER_HOME}/downloads/cmake-3.13.2-Linux-x86_64.sh --skip-license

RUN export PATH="${USER_HOME}/utils:${USER_HOME}/utils/bin:${PATH}"

RUN mkdir -p ${HIPE_EXTERNAL}/linux64
RUN mkdir -p ${HIPE_EXTERNAL}/downloads
RUN mkdir -p ${HIPE_EXTERNAL}/build

######
#Download all requierements
######


RUN mkdir -p ${HIPE_EXTERNAL}/downloads
WORKDIR ${HIPE_EXTERNAL}/downloads

# cuda
if [ ! -f cuda_10.0.run ]; then
	RUN wget -c -O cuda_10.0.run https://developer.nvidia.com/compute/cuda/10.0/Prod/local_installers/cuda_10.0.130_410.48_linux
fi
# Intel_TBB: intel-tbb-2018_U2.tar.gz
if [ ! -f intel-tbb-2018_U2.tar.gz ]; then
	RUN wget -c -O intel-tbb-2018_U2.tar.gz https://github.com/01org/tbb/archive/2018_U2.tar.gz
fi
WORKDIR ${HIPE_EXTERNAL}/downloads
# OpenCV: opencv-3.4.0.zip
if [ ! -f opencv-3.4.3.zip ]; then
	RUN wget -c -O opencv-3.4.3.zip https://github.com/opencv/opencv/archive/3.4.3.zip
fi
# OpenCV: opencv_contrib-3.4.0.zip
if [ ! -f opencv_contrib-3.4.3.zip ]; then
	RUN wget -c -O opencv_contrib-3.4.3.zip https://github.com/opencv/opencv_contrib/archive/3.4.3.zip
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

ENV PATH="/usr/local/cuda-10.0/bin${PATH:+:${PATH}}"
ENV PATH="${USER_HOME}/utils:${USER_HOME}/utils/bin:${PATH}"



######
#Build dependencies
######

WORKDIR ${script_dir}/utils


RUN bash build.sh -a "${HIPE_EXTERNAL}/downloads" \
	-b "${HIPE_EXTERNAL}/build" \
	-i "${HIPE_EXTERNAL}/linux64" \
	-g "${HIPE_EXTERNAL}/downloads" -d -c boost -n 8
	
RUN bash build.sh -a "${HIPE_EXTERNAL}/downloads" \
	-b "${HIPE_EXTERNAL}/build" \
	-i "${HIPE_EXTERNAL}/linux64" \
	-g "${HIPE_EXTERNAL}/downloads" -d -c dlib -n 8

WORKDIR ${script_dir}/utils

RUN bash build.sh -a "${HIPE_EXTERNAL}/downloads" \
	-b "${HIPE_EXTERNAL}/build" \
	-i "${HIPE_EXTERNAL}/linux64" \
	-g "${HIPE_EXTERNAL}/downloads" -d -c opencv -n 8

#install gflags
RUN wget --waitretry=3 -c -O ${USER_HOME}/downloads/gflags-2.2.2.zip https://github.com/gflags/gflags/archive/v2.2.2.zip
RUN unzip ${USER_HOME}/downloads/gflags-2.2.2.zip -d ${USER_HOME}/downloads/
RUN mkdir -p ${USER_HOME}/downloads/gflags-2.2.2/build/
WORKDIR ${USER_HOME}/downloads/gflags-2.2.2/build/
UN cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${HIPE_EXTERNAL}/linux64 ..
RUN make -j6 install

#install glogs
RUN wget --waitretry=3 -c -O ${USER_HOME}/downloads/glog-0.3.5.zip https://github.com/google/glog/archive/v0.3.5.zip
RUN unzip ${USER_HOME}/downloads/glog-0.3.5.zip -d ${USER_HOME}/downloads/
RUN mkdir -p ${USER_HOME}/downloads/glog-0.3.5/build/
WORKDIR ${USER_HOME}/downloads/glog-0.3.5/build/
RUN cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=${HIPE_EXTERNAL}/linux64 ..
RUN make -j6 install

#install boringssl
RUN wget --waitretry=3 -c -O ${USER_HOME}/downloads/boringssl.zip https://github.com/google/boringssl/archive/master.zip
RUN echo "Restart"
RUN unzip ${USER_HOME}/downloads/boringssl.zip -d ${USER_HOME}/downloads/
RUN mkdir -p ${USER_HOME}/downloads/boringssl-master/build/
WORKDIR ${USER_HOME}/downloads/boringssl-master/
COPY ${script_dir}/utils/cmake-boringssl.patch ./
RUN patch -p1 < cmake-boringssl.patch
WORKDIR ${USER_HOME}/downloads/boringssl-master/build
RUN cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${HIPE_EXTERNAL}/linux64 ..
RUN make -j6 install

RUN echo 'export PATH="$PATH:$HOME/bin"' >> ~/.bashrc

WORKDIR ${USER_HOME}/downloads/gflags-2.2.2/build/
RUN cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${HIPE_EXTERNAL}/linux64 -DBUILD_SHARED_LIBS=ON -DBUILD_gflags_nothreads_LIB=ON -DBUILD_STATIC_LIBS=ON ..
RUN make -j6 install

WORKDIR ${USER_HOME}/downloads
RUN wget --waitretry=3 -c -O ${USER_HOME}/downloads/websocketpp.zip https://github.com/zaphoyd/websocketpp/archive/master.zip
RUN unzip ${USER_HOME}/downloads/websocketpp.zip -d ${USER_HOME}/downloads/
RUN cp -rv ${USER_HOME}/downloads/websocketpp-master/websocketpp ${HIPE_EXTERNAL}/linux64/

RUN sudo apt-get install cuda-samples-10.0
RUN cuda-install-samples-10.0.sh ./
RUN echo 'export LD_LIBRARY_PATH="/usr/local/nvidia/lib64/:$LD_LIBRARY_PATH"' >> ~/.bashrc
WORKDIR ${USER_HOME}/downloads/NVIDIA_CUDA-10.0_Samples/1_Utilities/deviceQuery
RUN bash -c make
RUN ls -la /usr/local/cuda/lib64/stubs/libcuda.so.1 || true
RUN echo "PATH --> ${PATH}"
RUN echo "LD_LIBRARY_PATH --> ${LD_LIBRARY_PATH}"

RUN echo 'echo "The system is ready to get HIPE from source or from website"' >> ~/.bashrc
RUN echo 'export HIPE_EXTERNAL=${HIPE_EXTERNAL}' >> ~/.bashrc
WORKDIR ${USER_HOME}/




RUN cp ${script_dir}/mysql/install-webserver.sh ${USER_HOME}/install-webserver.sh
RUN rm -rf ${USER_HOME}/mysql_db
RUN mkdir -p ${USER_HOME}/mysql_db
RUN cp -f ${script_dir}/mysql/hipe.sql ${USER_HOME}/mysql_db/
WORKDIR ${USER_HOME}/

RUN chmod +x ${USER_HOME}/install-webserver.sh
RUN sudo bash ${USER_HOME}/install-webserver.sh ${USER_HOME}/mysql_db/ ${USER_NAME}

if [ ! -f /usr/local/bin/node ]; then
	RUN sudo ln -s /usr/bin/nodejs /usr/local/bin/node
fi

if [ ! -f /usr/local/bin/npm ]; then
	RUN sudo ln -s /usr/bin/npm /usr/local/bin/npm
fi


WORKDIR ${USER_HOME}/hipe/webserver/webserver-hipe
RUN pwd
RUN npm install

ENV PATH="${USER_HOME}/hipe/webserver/webserver-hipe/node_modules/.bin:${PATH}"
RUN echo $PATH
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
#WORKDIR /home/${USER}
#RUN git clone https://github.com/aneoconsulting/HIPE.git hipe
#WORKDIR /home/${USER}/hipe

#RUN bash compile-ALL.sh


