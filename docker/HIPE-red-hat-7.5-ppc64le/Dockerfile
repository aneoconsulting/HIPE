# Base this image on the official Nvidia CUDA image. We use the development
# version because we need to compile CUDA code.
FROM debian:buster-slim as qemu

RUN apt-get update && apt-get install --no-install-recommends -y qemu-user-static


FROM ppc64le/centos:7
LABEL maintainer "NVIDIA CORPORATION <cudatools@nvidia.com>"

COPY --from=qemu /usr/bin/qemu-ppc64le-static /usr/bin/qemu-ppc64le-static

RUN NVIDIA_GPGKEY_SUM=d1be581509378368edeec8c1eb2958702feedf3bc3d17011adbf24efacce4ab5 && \
    curl -fsSL https://developer.download.nvidia.com/compute/cuda/repos/rhel7/ppc64le/7fa2af80.pub | sed '/^Version/d' > /etc/pki/rpm-gpg/RPM-GPG-KEY-NVIDIA && \
    echo "$NVIDIA_GPGKEY_SUM  /etc/pki/rpm-gpg/RPM-GPG-KEY-NVIDIA" | sha256sum -c --strict -

ENV CUDA_REPO_URL=https://developer.nvidia.com/compute/cuda/8.0/Prod2/local_installers/cuda-repo-rhel7-8-0-local-ga2v2-8.0.61-1.ppc64le-rpm

RUN yum install -y wget

RUN wget $CUDA_REPO_URL

RUN rpm -i cuda-repo-rhel7-8-0-local-ga2v2-8.0.61-1.ppc64le-rpm

ENV CUDA_VERSION 8.0-ppc64le

#ENV CUDA_PKG_VERSION 9-1-$CUDA_VERSION-1
RUN yum install -y \
		cuda-toolkit-8-0.ppc64le && \
	    rm -rf /var/cache/yum/*

# nvidia-docker 1.0
LABEL com.nvidia.volumes.needed="nvidia_driver"
LABEL com.nvidia.cuda.version="${CUDA_VERSION}"

RUN echo "/usr/local/nvidia/lib" >> /etc/ld.so.conf.d/nvidia.conf && \
echo "/usr/local/nvidia/lib64" >> /etc/ld.so.conf.d/nvidia.conf

ENV CUDNN_VERSION 5.1.10
LABEL com.nvidia.cudnn.version="${CUDNN_VERSION}"

# cuDNN license: https://developer.nvidia.com/cudnn/license_agreement
RUN CUDNN_DOWNLOAD_SUM=663aac0328f821d90ae9c74ee43e90751706546c2ce769ea9c96f92864300af6 && \
	   curl -fsSL http://developer.download.nvidia.com/compute/redist/cudnn/v5.1/cudnn-8.0-linux-ppc64le-v5.1.tgz -O && \
       echo "$CUDNN_DOWNLOAD_SUM  cudnn-8.0-linux-ppc64le-v5.1.tgz" | sha256sum -c --strict - && \
       tar --no-same-owner -xzf cudnn-8.0-linux-ppc64le-v5.1.tgz -C /usr/local && \
       rm cudnn-8.0-linux-ppc64le-v5.1.tgz && \
       ldconfig



ENV PATH /usr/local/nvidia/bin:/usr/local/cuda/bin:${PATH}
ENV LD_LIBRARY_PATH /usr/local/nvidia/lib:/usr/local/nvidia/lib64

# nvidia-container-runtime
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility
ENV NVIDIA_REQUIRE_CUDA "cuda>=9.1"
							

# Install compatible system libraries to avoid compiling them in Hipe External.
RUN yum install -y \
   python \
  python-devel \
  gtk2.ppc64le \
  gtk2-devel.ppc64le \
  pkgconfig \
  xorg-x11-xauth \
  xorg-x11-apps \
  nano curl \
  sudo
  

# # Install generic packages required to build Hipe and Hipe External.
RUN yum install -y  \
  autoconf \
  automake \
  bsdtar \
  ca-certificates \
  gcc \
  wget \
  make \
  cmake

RUN mkdir /rpm
WORKDIR /rpm

RUN wget http://dl.fedoraproject.org/pub/epel/7/ppc64le/Packages/y/yasm-1.2.0-4.el7.ppc64le.rpm
RUN rpm -i yasm-1.2.0-4.el7.ppc64le.rpm

##Install Python 3.6
RUN yum install -y gcc openssl-devel bzip2-devel
RUN yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm

RUN yum install -y python34 python34-devel python34-numpy python34-pip
RUN yum -y groupinstall development
RUN python3.4 -m pip install PyYAML


#install gcc5 alternative
RUN yum install -y git libtool scl-utils

# Install pacackages required by libraries in Hipe External
# TODO: Finish sorting these into separate commands per library to facilitate
RUN yum install -y \
  libX11-devel \
  openssl-libs

# #Gstreamer Dependencies for opencv
RUN yum install -y \
	gstreamer1-devel.ppc64le \
	gstreamer1-plugins-bad-free.ppc64le \
	gstreamer1-plugins-bad-free-devel.ppc64le \
	gstreamer1-plugins-base.ppc64le \
	gstreamer1-plugins-base-devel.ppc64le \
	gstreamer1-plugins-base-tools.ppc64le \
	gstreamer1-plugins-good.ppc64le \
	gstreamer1-plugins-ugly-free.ppc64le \
	gstreamer1-plugins-ugly-free-devel.ppc64le \
	gstreamer1-rtsp-server.ppc64le \
	gstreamer1-rtsp-server-devel.ppc64le \
	gstreamer1-rtsp-server-devel-docs.noarch \
	gstreamer1-vaapi.ppc64le \
	gstreamer1-vaapi-devel.ppc64le

# 	libges-1.0-0 \
# 	gstreamer-tools \
# 	libges-1.0-dev \
# 	libavresample-dev \
# 	v4l-utils \
# 	libv4l-dev
	
# # Dependencies for building OpenCV
RUN yum install -y \
	glog-devel \
	glog \
	gflags-devel \
	gflags \
	protobuf-compiler \
	protobuf-devel \
	tesseract \
	leptonica-devel \
	leptonica \
	eigen3-devel \
	lapack-devel \
	lapack-static \
	lapack64-devel \
	lapack64-static \
	lapack \
	lapack64 \
	openblas-devel \
	openblas \
	openblas-openmp \
	openblas-openmp64 \
	atlas \
	atlas-devel \
	freetype \
	libtheora \
	libtheora-devel \
	libvorbis-devel \
	libvorbis \
	libicu \
	libicu-devel


 
# # Dependencies for building FFmpeg
# RUN apt-get install -y --no-install-recommends \
#   libfreetype6-dev \
#   libtheora-dev \
#   libvorbis-dev \
#   libavutil-dev \
#   libavcodec-dev \
#   libswscale-dev \
#   libavformat-dev \
#   libavutil-dev

#RUN python -m pip install numpy
#RUN python3   -m pip install numpy


# # Install the build deps for the compilation targets.
# #RUN apt-get build-dep \
# #  opencv \
# #  ffmpeg \

# # Copy the local build scripts and source files. The source files are downloaded
# # before building the image to avoid redundant downloads when rebuilding the
# # image. The entire hipe directory is not copied here because any changes to
# # the source files in hipe/hipe-group would invalidate the cache and trigger a rerun
# # off all commands from this point. This must be avoided because the following
# # compilation steps take a long time to complete.
# #COPY hipe/build /home/hipe/build
# #COPY hipe/hipe_external /home/hipe/hipe_external
# #RUN mkdir -p /home/hipe/hipe_external/linux64/build

# # Download before building the image to avoid redownloading the files with
# # every build.
# # TODO: Maybe make this optional. Given that the Git repo currently prompts for
# #       a name and password, all downloads cannot yet be automated anyway.
# #WORKDIR /home/hipe/hipe_external/linux64/build
# #RUN python3 prepare_download.py boost opencv dlib
# #RUN bash download_sources.sh





# ###########
# #DDU
# ##########

#####
# Create download area
####
RUN mkdir -p /home/hipe-group/downloads
WORKDIR /home/hipe-group/downloads

#####
# Create tools folder
#####

RUN mkdir -p /home/hipe-group/utils/
WORKDIR /home/hipe-group/utils
COPY utils/*.sh /home/hipe-group/utils/

ENV PATH="/home/hipe-group/utils:/home/hipe-group/utils/bin:${PATH}"
ENV PATH="/usr/local/bin:${PATH}"

RUN mkdir -p /home/hipe-group/3rdParty/hipe_external/linux64/install
RUN mkdir -p /home/hipe-group/3rdParty/hipe_external/linux64/downloads
RUN mkdir -p /home/hipe-group/3rdParty/hipe_external/linux64/build

#######
# Build gcc 5.4 (Yeah I know Gcc 4.8 has an issue with vector keyword altivec and the include of #include <future>)
######
RUN yum install -y svn texinfo-tex flex zip libgcc.i686 glibc-devel.i686
RUN yum install -y svn texinfo-tex flex zip libgcc glibc-devel
RUN mkdir /home/hipe-group/sourceInstallations
#RUN svn ls svn://gcc.gnu.org/svn/gcc/tags | grep gcc | grep release
WORKDIR /home/hipe-group/sourceInstallations
RUN mkdir /home/hipe-group/sourceInstallations/build
RUN svn co svn://gcc.gnu.org/svn/gcc/tags/gcc_5_4_0_release/
WORKDIR /home/hipe-group/sourceInstallationsgcc_5_4_0_release/
RUN bash ./contrib/download_prerequisites
WORKDIR /home/hipe-group/sourceInstallationsbuild/
RUN ../gcc_5_4_0_release/configure && make -j16 && sudo make -j16 install && echo "success"
RUN yum install -y which
WORKDIR /usr/bin
RUN mv cc cc-4.8.5
RUN mv gcc gcc-4.8.5
RUN mv g++ g++-4.8.5
RUN mv c++ c++-4.8.5
RUN ln -s /usr/local/bin/gcc cc
RUN ln -s /usr/local/bin/gcc gcc
RUN ln -s /usr/local/bin/g++ g++
RUN ln -s /usr/local/bin/g++ c++
RUN g++ --version
RUN echo "/usr/local/lib64" > usrLocalLib64.conf
RUN mv usrLocalLib64.conf /etc/ld.so.conf.d/
RUN ldconfig
RUN rm -rf /home/hipe-group/sourceInstallations/build


#######
# Compile and Install cmake 3.11
#######
RUN mkdir -p /home/hipe-group/utils
WORKDIR /home/hipe-group/utils
RUN git clone -b v3.11.4 https://gitlab.kitware.com/cmake/cmake.git
WORKDIR /home/hipe-group/utils/cmake
RUN bash ./bootstrap --parallel=16 && make -j20 && make install
ENV CMAKE_ROOT=/usr/local/share/cmake-3.11/


######
#Download all requierements
######

RUN mkdir -p /home/hipe-group/3rdParty/hipe_external/linux64/downloads
WORKDIR /home/hipe-group/3rdParty/hipe_external/linux64/downloads


# Intel_TBB: intel-tbb-2018_U2.tar.gz
RUN wget -c -O intel-tbb-2018_U2.tar.gz https://github.com/01org/tbb/archive/2018_U2.tar.gz
# OpenCV: opencv-3.4.0.zip
RUN wget -c -O opencv-3.4.2.zip https://github.com/opencv/opencv/archive/3.4.2.zip
# OpenCV: opencv_contrib-3.4.0.zip
RUN wget -c -O opencv_contrib-3.4.2.zip https://github.com/opencv/opencv_contrib/archive/3.4.2.zip
# Dlib: dlib-19.13.zip
RUN wget -c -O dlib-19.13.zip http://dlib.net/files/dlib-19.13.zip
# Boost: boost_1_62_0.tar.gz
RUN wget -c -O boost_1_62_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.62.0/boost_1_62_0.tar.gz
#NASM
RUN wget -c -O nasm-2.13.02.tar.gz http://www.nasm.us/pub/nasm/releasebuilds/2.13.02/nasm-2.13.02.tar.xz
#fdk-aac
RUN git clone https://github.com/mstorsjo/fdk-aac.git /home/hipe-group/3rdParty/hipe_external/linux64/build/fdk-aac
#x264
RUN git clone https://git.videolan.org/git/x264.git /home/hipe-group/3rdParty/hipe_external/linux64/build/x264
RUN cd /home/hipe-group/3rdParty/hipe_external/linux64/build/x264 && git reset --hard "2451a7282463f68e532f2eee090a70ab139bb3e7"

#libvpx
RUN git clone -b v1.6.1 https://chromium.googlesource.com/webm/libvpx /home/hipe-group/3rdParty/hipe_external/linux64/build/libvpx

#Ffmpeg OPUS 
run wget -O opus-1.1.4.tar.gz https://archive.mozilla.org/pub/opus/opus-1.1.4.tar.gz

#ffmpeg 3.3.2
run git clone -b n3.3.2 https://git.ffmpeg.org/ffmpeg.git /home/hipe-group/3rdParty/hipe_external/linux64/build/ffmpeg



RUN mkdir -p /home/hipe-group/deps/
WORKDIR /home/hipe-group/deps
COPY deps/*.sh /home/hipe-group/deps/

######
#Build dependencies
######
WORKDIR /home/hipe-group/deps
ENV PKG_CONFIG_PATH=/home/hipe-group/3rdParty/hipe_external/linux64/install/ffmpeg/lib/pkgconfig
RUN bash -x build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64/install" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c ffmpeg -n 20

WORKDIR /home/hipe-group/deps
RUN bash build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64/install" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c boost -n 20

RUN bash build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64/install" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c dlib -n 20

RUN bash build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64/install" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c opencv -n 20


##Now everything is built then add user hipe-group and change all rights for him
RUN useradd -d /home/hipe-group -ms /bin/bash -g root -G sudo -p hipe-group hipe-group
RUN groupadd hipe-group
RUN echo "hipe-group:hipe-group" | chpasswd && adduser hipe-group sudo
RUN echo "hipe-group ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

WORKDIR /home/hipe-group
RUN chown -hR hipe-group:hipe-group *
RUN chown -hR hipe-group:hipe-group /home/hipe-group

# COPY mysql/install-webserver.sh  /home/hipe-group/install-webserver.sh
# RUN mkdir /home/hipe-group/mysql_db
# COPY mysql/hipe.sql /home/hipe-group/mysql_db
# WORKDIR /home/hipe-group/

# RUN chmod +x /home/hipe-group/install-webserver.sh
# RUN bash /home/hipe-group/install-webserver.sh
# RUN ln -s /usr/bin/nodejs /usr/local/bin/node
# RUN ln -s /usr/bin/npm /usr/local/bin/npm

# EXPOSE 3306/tcp

# USER hipe-group

# COPY mysql/webserver-hipe.tar.gz /home/hipe-group/webserver-hipe.tar.gz
# RUN tar xvzf webserver-hipe.tar.gz
# WORKDIR /home/hipe-group/webserver-hipe

# RUN npm install

# ENV PATH="/home/hipe-group/webserver-hipe/node_modules/.bin:${PATH}"
# RUN gulp js
# RUN gulp css
# RUN gulp sass
# RUN gulp fonts

# #run node app.js

# RUN echo "echo 'start database for webserver'; sudo sudo service mysql restart; sleep 2; cd /home/hipe-group/webserver-hipe/; node app.js &" >> /home/hipe-group/.bashrc
# RUN echo "sleep 2; echo 'Webserver is started'" >> /home/hipe-group/.bashrc
# RUN echo "echo ''" >> /home/hipe-group/.bashrc
# RUN echo "echo '#################################################################'" >> /home/hipe-group/.bashrc
# RUN echo "echo ''" >> /home/hipe-group/.bashrc
# RUN echo "echo 'HELLO : To Start HIPE run this script : '" >> /home/hipe-group/.bashrc
# RUN echo "echo 'bash /home/hipe-group/hipe/install/hipe-core/bin/Release/startHipe.sh'" >> /home/hipe-group/.bashrc
# RUN echo "echo ''" >> /home/hipe-group/.bashrc
# RUN echo "echo '#################################################################'" >> /home/hipe-group/.bashrc
# RUN echo "echo ''" >> /home/hipe-group/.bashrc

# # Build Hipe.
# WORKDIR /home/hipe-group
# ENV HIPE_EXTERNAL /home/hipe-group/3rdParty/hipe_external
# RUN git clone https://github.com/aneoconsulting/HIPE.git hipe
# WORKDIR /home/hipe-group/hipe

# RUN bash compile-ALL.sh

# WORKDIR /home/hipe-group

