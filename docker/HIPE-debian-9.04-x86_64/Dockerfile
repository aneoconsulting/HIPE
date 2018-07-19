# docker-debian-cuda - Debian 9 with CUDA Toolkit and cuDNN and without CUDA Driver

FROM debian:stretch

ENV DEBIAN_FRONTEND noninteractive

# Update system and package lists.
RUN apt-get update


RUN apt-get update -qq \
 && apt-get install --no-install-recommends -y \
    # install essentials
    gnupg2 \
    wget \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

# install from nvidia repositories
RUN wget -nv -P /root/manual http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1704/x86_64/7fa2af80.pub \
 && echo "47217c49dcb9e47a8728b354450f694c9898cd4a126173044a69b1e9ac0fba96  /root/manual/7fa2af80.pub" | sha256sum -c --strict - \
 && apt-key add /root/manual/7fa2af80.pub \
 && wget -nv -P /root/manual http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1704/x86_64/cuda-repo-ubuntu1704_9.1.85-1_amd64.deb \
 && dpkg -i /root/manual/cuda-repo-ubuntu1704_9.1.85-1_amd64.deb \
 && echo 'deb http://developer.download.nvidia.com/compute/machine-learning/repos/ubuntu1604/x86_64 /' > /etc/apt/sources.list.d/nvidia-ml.list \
 && rm -rf /root/manual

RUN apt-get update -qq \
 && apt-get install --no-install-recommends -y \
    # install cuda toolkit
    cuda-toolkit-9-0 \
    cuda-toolkit-9-1 \
    # install cudnn
    libcudnn7=7.0.5.15-1+cuda9.0 \
    libcudnn7-dev=7.0.5.15-1+cuda9.0 \
    libcudnn7=7.0.5.15-1+cuda9.1 \
    libcudnn7-dev=7.0.5.15-1+cuda9.1 \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

# fix issues with shared objects
RUN ls /usr/local/cuda-9.1/targets/x86_64-linux/lib/stubs/* | xargs -I{} ln -s {} /usr/lib/x86_64-linux-gnu/ \
 && ln -s libcuda.so /usr/lib/x86_64-linux-gnu/libcuda.so.1 \
 && ln -s libnvidia-ml.so /usr/lib/x86_64-linux-gnu/libnvidia-ml.so.1


# Enable Ubuntu multiverse packages. This also enables the matching deb-src
# repositories.
RUN sed -i 's@^#* *\(deb.* multiverse\)$@\1@' /etc/apt/sources.list

run apt-get update

# Install compatible system libraries to avoid compiling them in Hipe External.
RUN apt-get install -y --no-install-recommends \
  python    \
  python-dev \
  liblivemedia-dev \
  libgtk2.0 \
  libgtk2.0-dev \
  pkg-config \
  xauth \
  x11-apps \
  nano curl
  

# Install generic packages required to build Hipe and Hipe External.
RUN apt-get install -y --no-install-recommends \
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
	libges-1.0-dev \
	libgstreamer-plugins-bad1.0-dev \
	libgstreamer-plugins-bad1.0-0 \
	gstreamer1.0-plugins-ugly \
	gstreamer1.0-vaapi \
	gstreamer1.0-libav \
	gstreamer1.0-vaapi \
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
	libicu57 \
	libicu-dev

RUN python2.7 -m pip install numpy
RUN python3   -m pip install numpy

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

WORKDIR /home/hipe-group

RUN mkdir -p /home/hipe-group/hipe_external/linux64/install
RUN mkdir -p /home/hipe-group/hipe_external/linux64/downloads
RUN mkdir -p /home/hipe-group/hipe_external/linux64/build

ENV HIPE_EXTERNAL /home/hipe-group/hipe_external


######
#Download all requierements
######

RUN mkdir -p /home/hipe-group/hipe_external/linux64/downloads
WORKDIR /home/hipe-group/hipe_external/linux64/downloads


# Intel_TBB: intel-tbb-2018_U2.tar.gz
RUN wget -c -O intel-tbb-2018_U2.tar.gz https://github.com/01org/tbb/archive/2018_U2.tar.gz
# OpenCV: opencv-3.4.0.zip
RUN wget -c -O opencv-3.4.0.zip https://github.com/opencv/opencv/archive/3.4.0.zip
# OpenCV: opencv_contrib-3.4.0.zip
RUN wget -c -O opencv_contrib-3.4.0.zip https://github.com/opencv/opencv_contrib/archive/3.4.0.zip
# Dlib: dlib-19.13.zip
RUN wget -c -O dlib-19.13.zip http://dlib.net/files/dlib-19.13.zip
# Boost: boost_1_62_0.tar.gz
RUN wget -c -O boost_1_62_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.62.0/boost_1_62_0.tar.gz

######
#Build dependencies
######

WORKDIR /home/hipe-group/utils
RUN bash build.sh -a "/home/hipe-group/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/hipe_external/linux64/build" \
	-i "/home/hipe-group/hipe_external/linux64/install" \
	-g "/home/hipe-group/hipe_external/linux64/downloads" -d -c boost -n 8


RUN bash build.sh -a "/home/hipe-group/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/hipe_external/linux64/build" \
	-i "/home/hipe-group/hipe_external/linux64/install" \
	-g "/home/hipe-group/hipe_external/linux64/downloads" -d -c dlib -n 8


RUN bash build.sh -a "/home/hipe-group/hipe_external/linux64/downloads" \
	-b "/home/hipe-group/hipe_external/linux64/build" \
	-i "/home/hipe-group/hipe_external/linux64/install" \
	-g "/home/hipe-group/hipe_external/linux64/downloads" -d -c opencv -n 8

# Dependencies for building Boost
RUN apt-get install -y --no-install-recommends \
	sudo
	
##Now everything is built then add user hipe-group and change all rights for him
RUN useradd -d /home/hipe-group -ms /bin/bash -g root -G sudo -p hipe-group hipe-group
RUN groupadd hipe-group
RUN echo "hipe-group:hipe-group" | chpasswd && adduser hipe-group sudo
RUN echo "hipe-group ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

WORKDIR /home/hipe-group
RUN chown -hR hipe-group:hipe-group *

RUN chown -hR hipe-group:hipe-group /home/hipe-group

COPY mysql/install-webserver.sh  /home/hipe-group/install-webserver.sh
COPY mysql/install_mysql.sh  /home/hipe-group/install_mysql.sh
RUN mkdir /home/hipe-group/mysql_db
COPY mysql/hipe.sql /home/hipe-group/mysql_db
WORKDIR /home/hipe-group/
RUN chmod +x /home/hipe-group/install_mysql.sh
RUN bash /home/hipe-group/install_mysql.sh
RUN chmod +x /home/hipe-group/install-webserver.sh
RUN bash /home/hipe-group/install-webserver.sh
RUN ln -s /usr/bin/nodejs /usr/local/bin/node
RUN ln -s /usr/bin/npm /usr/local/bin/npm


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


WORKDIR /home/hipe-group


RUN git clone -v https://github.com/aneoconsulting/HIPE.git hipe

# Build Hipe.
WORKDIR /home/hipe-group/hipe
ENV HIPE_EXTERNAL /home/hipe-group/hipe_external
RUN bash compile-ALL.sh

WORKDIR /home/hipe-group

RUN echo "echo 'start database for webserver';" >> /home/hipe-group/.bashrc
RUN echo "sudo mysqld --default-authentication-plugin=mysql_native_password --user=root &"  >> /home/hipe-group/.bashrc
RUN echo "sleep 5; cd /home/hipe-group/webserver-hipe/; node app.js &" >> /home/hipe-group/.bashrc
RUN echo "sleep 2; echo 'Webserver is started'" >> /home/hipe-group/.bashrc
RUN echo "echo ''" >> /home/hipe-group/.bashrc
RUN echo "echo '#################################################################'" >> /home/hipe-group/.bashrc
RUN echo "echo ''" >> /home/hipe-group/.bashrc
RUN echo "echo 'HELLO : To Start HIPE run this script : '" >> /home/hipe-group/.bashrc
RUN echo "echo 'bash /home/hipe-group/hipe/install/hipe-core/bin/Release/startHipe.sh'" >> /home/hipe-group/.bashrc
RUN echo "echo ''" >> /home/hipe-group/.bashrc
RUN echo "echo '#################################################################'" >> /home/hipe-group/.bashrc
RUN echo "echo ''" >> /home/hipe-group/.bashrc

EXPOSE 3306/tcp
EXPOSE 9999/udp
EXPOSE 9090/tcp
EXPOSE 3000/tcp

#RUN mkdir /home/hipe-group/hipe-store
#VOLUME /home/hipe-group/hipe-store
