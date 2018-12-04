# Base this image on the official Nvidia CUDA image. We use the development
# version because we need to compile CUDA code.
FROM nvidia/cuda:8.0-devel-ubuntu16.04

# Enable Ubuntu multiverse packages. This also enables the matching deb-src
# repositories.
RUN sed -i 's@^#* *\(deb.* multiverse\)$@\1@' /etc/apt/sources.list

# Update system and package lists.
RUN apt-get update

# Install compatible system libraries to avoid compiling them in Hipe External.
RUN apt-get install -y --no-install-recommends \
  python2.7 \
  python2.7-dev \
  liblivemedia-dev \
  libgtk2.0 \
  libgtk2.0-dev \
  pkg-config \
  xauth \
  x11-apps \
  nano curl \
  sudo
  

# Install generic packages required to build Hipe and Hipe External.
RUN apt-get install -y --no-install-recommends \
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
  make

# Install pacackages required by libraries in Hipe External
# TODO: Finish sorting these into separate commands per library to facilitate
#       maintenance (additions/removals).
RUN apt-get install -y --no-install-recommends \
  libcurl4-openssl-dev \
  libssl-dev \
  libx11-dev \
  python3-dev \
  python-dev \
  python-numpy \
  python3-numpy \
  python-setuptools \
  python3-pip \
  python-pip

#Gstreamer Dependencies for opencv
RUN apt-get install -y --no-install-recommends \
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
	libv4l-dev
	
# Dependencies for building OpenCV
RUN apt-get install -y --no-install-recommends \
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
  libatlas-dev
  
# Dependencies for building FFmpeg
RUN apt-get install -y --no-install-recommends \
  libfreetype6-dev \
  libtheora-dev \
  libvorbis-dev \
  libavutil-dev \
  libavcodec-dev \
  libswscale-dev \
  libavformat-dev \
  libavutil-dev

# Dependencies for building Boost
RUN apt-get install -y --no-install-recommends \
	libicu55 \
	libicu-dev




RUN python2.7 -m pip install numpy
RUN python3   -m pip install numpy


# Install the build deps for the compilation targets.
#RUN apt-get build-dep \
#  opencv \
#  ffmpeg \

# Copy the local build scripts and source files. The source files are downloaded
# before building the image to avoid redundant downloads when rebuilding the
# image. The entire hipe directory is not copied here because any changes to
# the source files in hipe/hipe-group would invalidate the cache and trigger a rerun
# off all commands from this point. This must be avoided because the following
# compilation steps take a long time to complete.
#COPY hipe/build /home/hipe/build
#COPY hipe/hipe_external /home/hipe/hipe_external
#RUN mkdir -p /home/hipe/hipe_external/linux64/build

# Download before building the image to avoid redownloading the files with
# every build.
# TODO: Maybe make this optional. Given that the Git repo currently prompts for
#       a name and password, all downloads cannot yet be automated anyway.
#WORKDIR /home/hipe/hipe_external/linux64/build
#RUN python3 prepare_download.py boost opencv dlib
#RUN bash download_sources.sh





###########
#DDU
##########

#####
# Create download area
####
RUN mkdir -p /home/hipe-group/downloads
WORKDIR /home/hipe-group/downloads

RUN wget https://cmake.org/files/v3.11/cmake-3.11.4-Linux-x86_64.sh



#####
# Create tools folder
#####

RUN mkdir -p /home/hipe-group/utils/
WORKDIR /home/hipe-group/utils
COPY utils/*.sh /home/hipe-group/utils/
RUN bash ../downloads/cmake-3.11.4-Linux-x86_64.sh --skip-license

ENV PATH="/home/hipe-group/utils:/home/hipe-group/utils/bin:${PATH}"

RUN mkdir -p /home/hipe-group/3rdParty/hipe_external/linux64/install
RUN mkdir -p /home/hipe-group/3rdParty/hipe_external/linux64/downloads
RUN mkdir -p /home/hipe-group/3rdParty/hipe_external/linux64/build

######
#Download all requierements
######

RUN mkdir -p /home/hipe-group/3rdParty/hipe_external/linux64/downloads
WORKDIR /home/hipe-group/3rdParty/hipe_external/linux64/downloads


# Intel_TBB: intel-tbb-2018_U2.tar.gz
RUN wget -c -O intel-tbb-2018_U2.tar.gz https://github.com/01org/tbb/archive/2018_U2.tar.gz
# OpenCV: opencv-3.4.0.zip
RUN wget -c -O opencv-3.4.2.zip https://github.com/opencv/opencv/archive/3.4.0.zip
# OpenCV: opencv_contrib-3.4.0.zip
RUN wget -c -O opencv_contrib-3.4.2.zip https://github.com/opencv/opencv_contrib/archive/3.4.0.zip
# Dlib: dlib-19.13.zip
RUN wget -c -O dlib-19.16.zip http://dlib.net/files/dlib-19.13.zip
# Boost: boost_1_62_0.tar.gz
RUN wget -c -O boost_1_66_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.62.0/boost_1_66_0.tar.gz





######
#Build dependencies
######

WORKDIR /home/hipe-group/utils

RUN bash google-deps.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64/install" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c -n 8
	

RUN bash build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64/install" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c boost -n 8
	
RUN bash build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64/install" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c dlib -n 8

RUN bash build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64/install" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c opencv -n 8

RUN bash build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64/install" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c glog -n 8

RUN bash build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64/install" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c gflags -n 8

RUN bash build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64/install" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c borringssl -n 8



##Now everything is built then add user hipe-group and change all rights for him
RUN useradd -d /home/hipe-group -ms /bin/bash -g root -G sudo -p hipe-group hipe-group
RUN groupadd hipe-group
RUN echo "hipe-group:hipe-group" | chpasswd && adduser hipe-group sudo
RUN echo "hipe-group ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

WORKDIR /home/hipe-group
RUN chown -hR hipe-group:hipe-group *
RUN chown -hR hipe-group:hipe-group /home/hipe-group

COPY mysql/install-webserver.sh  /home/hipe-group/install-webserver.sh
RUN mkdir /home/hipe-group/mysql_db
COPY mysql/hipe.sql /home/hipe-group/mysql_db
WORKDIR /home/hipe-group/

RUN chmod +x /home/hipe-group/install-webserver.sh
RUN bash /home/hipe-group/install-webserver.sh
RUN ln -s /usr/bin/nodejs /usr/local/bin/node
RUN ln -s /usr/bin/npm /usr/local/bin/npm

EXPOSE 3306/tcp

USER hipe-group

COPY mysql/webserver-hipe.tar.gz /home/hipe-group/webserver-hipe.tar.gz
RUN tar xvzf webserver-hipe.tar.gz
WORKDIR /home/hipe-group/webserver-hipe

RUN npm install

ENV PATH="/home/hipe-group/webserver-hipe/node_modules/.bin:${PATH}"
RUN gulp js
RUN gulp css
RUN gulp sass
RUN gulp fonts

#run node app.js

RUN echo "echo 'start database for webserver'; sudo sudo service mysql restart; sleep 2; cd /home/hipe-group/webserver-hipe/; node app.js &" >> /home/hipe-group/.bashrc
RUN echo "sleep 2; echo 'Webserver is started'" >> /home/hipe-group/.bashrc
RUN echo "echo ''" >> /home/hipe-group/.bashrc
RUN echo "echo '#################################################################'" >> /home/hipe-group/.bashrc
RUN echo "echo ''" >> /home/hipe-group/.bashrc
RUN echo "echo 'HELLO : To Start HIPE run this script : '" >> /home/hipe-group/.bashrc
RUN echo "echo 'bash /home/hipe-group/hipe/install/hipe-core/bin/Release/startHipe.sh'" >> /home/hipe-group/.bashrc
RUN echo "echo ''" >> /home/hipe-group/.bashrc
RUN echo "echo '#################################################################'" >> /home/hipe-group/.bashrc
RUN echo "echo ''" >> /home/hipe-group/.bashrc

# Build Hipe.
WORKDIR /home/hipe-group
ENV HIPE_EXTERNAL /home/hipe-group/3rdParty/hipe_external
RUN git clone https://github.com/aneoconsulting/HIPE.git hipe
WORKDIR /home/hipe-group/hipe

RUN bash compile-ALL.sh

WORKDIR /home/hipe-group

