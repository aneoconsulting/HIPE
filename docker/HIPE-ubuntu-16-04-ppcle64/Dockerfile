# Base this image on the official Nvidia CUDA image. We use the development
# version because we need to compile CUDA code.
FROM debian:buster-slim as qemu

RUN apt-get update && apt-get install --no-install-recommends -y qemu-user-static

FROM ppc64le/ubuntu:xenial-20181113

FROM nvidia/cuda-ppc64le:9.2-runtime-ubuntu16.04
FROM nvidia/cuda-ppc64le:9.2-devel-ubuntu16.04
FROM nvidia/cuda-ppc64le:9.2-cudnn7-devel-ubuntu16.04

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

RUN wget -c -O cmake-3.12.4.tar.gz https://github.com/Kitware/CMake/releases/download/v3.12.4/cmake-3.12.4.tar.gz
RUN tar xvzf cmake-3.12.4.tar.gz
WORKDIR cmake-3.12.4
RUN bash ./configure --prefix=/home/hipe-group/utils/
RUN make -j32 install


#####
# Create tools folder
#####

RUN mkdir -p /home/hipe-group/utils/
WORKDIR /home/hipe-group/utils
COPY utils/*.sh /home/hipe-group/utils/


ENV PATH="/home/hipe-group/utils:/home/hipe-group/utils/bin:${PATH}"

RUN mkdir -p /home/hipe-group/3rdParty/hipe_external/linux64
RUN mkdir -p /home/hipe-group/3rdParty/hipe_external/linux64/downloads
RUN mkdir -p /home/hipe-group/3rdParty/hipe_external/linux64/build

######
#Download all requierements
######

RUN mkdir -p /home/hipe-group/3rdParty/hipe_external/linux64/downloads
WORKDIR /home/hipe-group/3rdParty/hipe_external/linux64/downloads


# Intel_TBB: intel-tbb-2018_U2.tar.gz
RUN wget -c -O intel-tbb-2018_U2.tar.gz https://github.com/01org/tbb/archive/2018_U2.tar.gz
# OpenCV: opencv-3.4.3.zip
RUN wget -c -O opencv-3.4.3.zip https://github.com/opencv/opencv/archive/3.4.3.zip
# OpenCV: opencv_contrib-3.4.0.zip
RUN wget -c -O opencv_contrib-3.4.3.zip https://github.com/opencv/opencv_contrib/archive/3.4.3.zip
# Dlib: dlib-19.13.zip
RUN wget -c -O dlib-19.16.zip http://dlib.net/files/dlib-19.16.zip
# Boost: boost_1_66_0.tar.gz
RUN wget -c -O boost_1_66_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.66.0/boost_1_66_0.tar.gz





######
#Build dependencies
######

WORKDIR /home/hipe-group/utils

#RUN bash google-deps.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
#	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
#	-i "/home/hipe-group/3rdParty/hipe_external/linux64" \
#	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c -n 48
	

RUN bash build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
 	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c boost -n 48

#need fix for consuming_buffer.hpp
WORKDIR /home/hipe-group/3rdParty/hipe_external/linux64/boost
COPY utils/boost_1_66.patch /home/hipe-group/3rdParty/hipe_external/linux64/boost/
RUN patch -p0 < boost_1_66.patch
WORKDIR /home/hipe-group/utils
	
RUN bash build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c dlib -n 48

#need fix for consuming_buffer.hpp
WORKDIR /home/hipe-group/3rdParty/hipe_external/linux64/dlib
COPY utils/dlib_19_6.patch /home/hipe-group/3rdParty/hipe_external/linux64/dlib
RUN patch -p0 < dlib_19_6.patch
WORKDIR /home/hipe-group/utils


RUN bash build.sh -a "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/3rdParty/hipe_external/linux64/build" \
	-i "/home/hipe-group/3rdParty/hipe_external/linux64" \
	-g "/home/hipe-group/3rdParty/hipe_external/linux64/downloads" -d -c opencv -n 48



##Now everything is built then add user hipe-group and change all rights for him
RUN useradd -d /home/hipe-group -ms /bin/bash -g root -G sudo -p hipe-group hipe-group
RUN groupadd hipe-group
RUN echo "hipe-group:hipe-group" | chpasswd && adduser hipe-group sudo
RUN echo "hipe-group ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

WORKDIR /home/hipe-group
RUN chown -hR hipe-group:hipe-group *
RUN chown -hR hipe-group:hipe-group /home/hipe-group

#COPY mysql/install-webserver.sh  /home/hipe-group/install-webserver.sh
#RUN mkdir /home/hipe-group/mysql_db
#COPY mysql/hipe.sql /home/hipe-group/mysql_db
WORKDIR /home/hipe-group/

#RUN chmod +x /home/hipe-group/install-webserver.sh
#RUN bash /home/hipe-group/install-webserver.sh
#RUN ln -s /usr/bin/nodejs /usr/local/bin/node
#RUN ln -s /usr/bin/npm /usr/local/bin/npm

#EXPOSE 3306/tcp

USER hipe-group

#COPY mysql/webserver-hipe.tar.gz /home/hipe-group/webserver-hipe.tar.gz
#RUN tar xvzf webserver-hipe.tar.gz
#WORKDIR /home/hipe-group/webserver-hipe

#RUN npm install

#ENV PATH="/home/hipe-group/webserver-hipe/node_modules/.bin:${PATH}"
#RUN gulp js
#RUN gulp css
#RUN gulp sass
#RUN gulp fonts

#run node app.js

#RUN echo "echo 'start database for webserver'; sudo sudo service mysql restart; sleep 2; cd /home/hipe-group/webserver-hipe/; node app.js &" >> /home/hipe-group/.bashrc
#RUN echo "sleep 2; echo 'Webserver is started'" >> /home/hipe-group/.bashrc
#RUN echo "echo ''" >> /home/hipe-group/.bashrc
#RUN echo "echo '#################################################################'" >> /home/hipe-group/.bashrc
#RUN echo "echo ''" >> /home/hipe-group/.bashrc
RUN echo "echo 'HELLO : To Start HIPE run this script : '" >> /home/hipe-group/.bashrc
RUN echo "echo 'bash /home/hipe-group/hipe/install/hipe-core/bin/Release/startHipe.sh'" >> /home/hipe-group/.bashrc
RUN echo "echo ''" >> /home/hipe-group/.bashrc
RUN echo "echo '#################################################################'" >> /home/hipe-group/.bashrc
RUN echo "echo ''" >> /home/hipe-group/.bashrc

# Build Hipe.
WORKDIR /home/hipe-group
ENV HIPE_EXTERNAL /home/hipe-group/3rdParty/hipe_external
#RUN git clone https://github.com/aneoconsulting/HIPE.git hipe
WORKDIR /home/hipe-group/hipe



WORKDIR /home/hipe-group

RUN sudo apt-get install -y unzip

#install gflags
RUN wget -c -O /home/hipe-group/downloads/gflags-2.2.2.zip https://github.com/gflags/gflags/archive/v2.2.2.zip
RUN unzip /home/hipe-group/downloads/gflags-2.2.2.zip -d /home/hipe-group/downloads/
RUN mkdir -p /home/hipe-group/downloads/gflags-2.2.2/build/
WORKDIR /home/hipe-group/downloads/gflags-2.2.2/build/
RUN cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/home/hipe-group/3rdParty/hipe_external/linux64 ..
RUN make -j64 install

#install glogs
RUN wget -c -O /home/hipe-group/downloads/glog-0.3.5.zip https://github.com/google/glog/archive/v0.3.5.zip
RUN unzip /home/hipe-group/downloads/glog-0.3.5.zip -d /home/hipe-group/downloads/
RUN mkdir -p /home/hipe-group/downloads/glog-0.3.5/build/
WORKDIR /home/hipe-group/downloads/glog-0.3.5/build/
RUN cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/home/hipe-group/3rdParty/hipe_external/linux64 ..
RUN make -j64 install

#install boringssl
RUN sudo apt-get -y install golang-go
RUN wget -c -O /home/hipe-group/downloads/boringssl.zip https://github.com/google/boringssl/archive/master.zip
RUN echo "Restart"
RUN unzip /home/hipe-group/downloads/boringssl.zip -d /home/hipe-group/downloads/
RUN mkdir -p /home/hipe-group/downloads/boringssl-master/build/
WORKDIR /home/hipe-group/downloads/boringssl-master/
COPY ./utils/cmake-boringssl.patch /home/hipe-group/downloads/boringssl-master/
RUN patch -p1 < cmake-boringssl.patch
WORKDIR /home/hipe-group/downloads/boringssl-master/build
RUN cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/home/hipe-group/3rdParty/hipe_external/linux64 ..
RUN make -j64 install


#build python 3.6.7
RUN sudo apt install -y libssl-dev zlib1g-dev libncurses5-dev libncursesw5-dev libreadline-dev libsqlite3-dev
RUN sudo apt install -y libgdbm-dev libdb5.3-dev libbz2-dev libexpat1-dev liblzma-dev tk-dev
RUN wget -c -O /home/hipe-group/downloads/Python-3.6.7.tar.xz https://www.python.org/ftp/python/3.6.7/Python-3.6.7.tar.xz
RUN tar xf /home/hipe-group/downloads/Python-3.6.7.tar.xz -C /home/hipe-group/downloads/
WORKDIR /home/hipe-group/downloads/Python-3.6.7

RUN ./configure --enable-shared --enable-optimizations --prefix=/home/hipe-group/python-3.6.7/
RUN make -j64 install

#Set Path for python
ENV PATH="/home/hipe-group/python-3.6.7/bin:${PATH}"
ENV LD_LIBRARY_PATH="/home/hipe-group/python-3.6.7/lib/:${LD_LIBRARY_PATH}"

RUN sudo apt-get -y install build-essential libjpeg-dev
RUN sudo apt-get install -y zip

ENV PATH="/usr/local/cuda-9.2/bin${PATH:+:${PATH}}"

RUN pip3.6 install  six numpy wheel mock pillow

RUN pip3.6 install  keras --no-deps
RUN wget -c -O /home/hipe-group/downloads/bazel-0.19.2.zip https://github.com/bazelbuild/bazel/releases/download/0.19.2/bazel-0.19.2-dist.zip
RUN unzip /home/hipe-group/downloads/bazel-0.19.2.zip -d /home/hipe-group/downloads/bazel-0.19.2
RUN sudo apt-get -y install openjdk-8-jdk
WORKDIR /home/hipe-group/downloads/bazel-0.19.2/
RUN bash compile.sh
RUN sudo cp /home/hipe-group/downloads/bazel-0.19.2/output/bazel /usr/bin/

RUN sudo chmod +x /usr/bin/bazel
WORKDIR /home/hipe-group/downloads/
RUN wget https://developer.download.nvidia.com/compute/machine-learning/repos/ubuntu1604/ppc64el/nvidia-machine-learning-repo-ubuntu1604_1.0.0-1_ppc64el.deb
RUN sudo dpkg -i nvidia-machine-learning-repo-ubuntu1604_1.0.0-1_ppc64el.deb
RUN sudo apt install libnccl2=2.3.7-1+cuda9.2 libnccl-dev=2.3.7-1+cuda9.2


ENV PATH="/home/hipe-group/downloads/bin:${PATH}"
RUN pip3.6 install keras_applications==1.0.5 --no-deps
RUN pip3.6 install keras_preprocessing==1.0.3 --no-deps

ENV LD_LIBRARY_PATH="/usr/local/lib:/usr/local/cuda/lib64:/usr/local/cuda/lib64/stubs:${LD_LIBRARY_PATH}"
ENV	PATH="/usr/local/cuda/bin:${PATH}"
RUN sudo ln -s /usr/local/cuda-9.2/lib64/stubs/libcuda.so /usr/local/cuda-9.2/lib64/stubs/libcuda.so.1
RUN sudo apt-get -y install gcc-4.8 g++-4.8
RUN echo "Running Git clone for tensorflow                           go..."
RUN git clone https://github.com/tensorflow/tensorflow.git
WORKDIR /home/hipe-group/downloads/tensorflow
RUN git checkout r1.12
RUN cp /home/hipe-group/downloads/tensorflow/tools/bazel.rc /home/hipe-group/downloads/tensorflow/.bazelrc
RUN sh -c '/bin/echo -e "/home/hipe-group/python-3.6.7/bin/python3.6\n/home/hipe-group/python-3.6.7/lib/python3.6/site-packages\nN\nY\nN\nN\nY\n9.2\n/usr/local/cuda-9.2\n7.2.1\n/usr\nN\n2.3.7\n5.0,5.2,7.0\nN\n/usr/bin/gcc-4.8\nN\n-mcpu=power8\nN\n"'|  ./configure
COPY utils/tf_boringssl.patch /home/hipe-group/downloads/tensorflow/
#RUN patch -p1 < tf_boringssl.patch
RUN bazel build --jobs 64 --config=opt --config=cuda //tensorflow/tools/pip_package:build_pip_package
RUN bazel-bin/tensorflow/tools/pip_package/build_pip_package tensorflow_pkg
WORKDIR /home/hipe-group/downloads/tensorflow/tensorflow_pkg
RUN sudo apt-get install -y libhdf5-dev
RUN pip3.6 install  pyYAML scipy
#RUN pip3.6 install tensorflow*.whl


#RUN echo 'export PATH="$PATH:$HOME/bin"' >> ~/.bashrc

##GO Compile Hipe now
RUN find -iname "*.cmake" -exec sed -e 's/CUDA_MAJOR "10"/CUDA_MAJOR "9"/g' -i {} \;
RUN find -iname "*.cmake" -exec sed -e 's/CUDA_MINOR "0"/CUDA_MINOR "2"/g' -i {} \;
WORKDIR /home/hipe-group/downloads/gflags-2.2.2/build/
RUN cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/home/hipe-group/3rdParty/hipe_external/linux64 -DBUILD_SHARED_LIBS=ON -DBUILD_gflags_nothreads_LIB=ON -DBUILD_STATIC_LIBS=ON ..
RUN make -j64 install

WORKDIR /home/hipe-group/downloads
RUN wget -c https://public.dhe.ibm.com/software/server/POWER/Linux/mldl/ubuntu/mldl-repo-local_4.0.0_ppc64el.deb
RUN sudo dpkg -i mldl-repo-*.deb
RUN sudo apt-get update
RUN sudo apt-get install caffe-ibm
RUN wget -c -O /home/hipe-group/downloads/websocketpp.zip https://github.com/zaphoyd/websocketpp/archive/master.zip
RUN unzip /home/hipe-group/downloads/websocketpp.zip -d /home/hipe-group/downloads/
RUN cp -rv /home/hipe-group/downloads/websocketpp-master/websocketpp /home/hipe-group/3rdParty/hipe_external/linux64/

#RUN bash compile-ALL.sh
