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

#install gcc5 alternative
# Install pacackages required by libraries in Hipe External

RUN yum install -y gcc openssl-devel bzip2-devel
RUN yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm

RUN yum -y groupinstall development

RUN yum install -y \
	python34 python34-devel python34-numpy python34-pip python2-pip \
	git libtool scl-utils \
  	libX11-devel \
    openssl-libs \
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
	gstreamer1-vaapi-devel.ppc64le \
	python2-numpy \
	glog-devel \
	glog \
	gflags-devel \
	gflags \
	protobuf-compiler \
	protobuf-devel \
	tesseract \
	tesseract-devel \
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

#####
# Install numpy and PyYAML
####
RUN python3.4 -m pip install PyYAML
RUN python -m pip install numpy
RUN python3.4 -m pip install numpy


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

