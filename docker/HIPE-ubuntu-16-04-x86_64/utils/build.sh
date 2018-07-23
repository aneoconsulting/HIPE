#!/bin/bash

# README
# See the separate README for troubleshooting suggestions.



# Exit on error or use of unset variable.
set -eu
shopt -s nullglob

self_="$(readlink -f "$0")"
script_dir="${self_%/*}"

ARCHIVE_DIRECTORY="$script_dir/downloads/archives"
BUILD_DIRECTORY="$script_dir/../build"
INSTALL_DIRECTORY="$script_dir/install"
GIT_DIRECTORY="$script_dir/../downloads"
PATCH_DIRECTORY="$script_dir/downloads/archives"
OPENCV_CUDA=ON
TMPDIR="$script_dir/tmp"
N_THREADS=$(($(nproc) + 1))
DELETE_BUILD_FILES=false


function message()
{
  local text_="$*"
#   if [[ ${TERM:-dumb} != dumb ]] && command -v tput >/dev/null
#   then
#     local width_=$(tput cols)
#     local marker_="$(printf "%${width_}s")"
#     marker_="${marker_// /#}"
#   else
#     local marker_='####################'
#   fi
  local prefix="MESSAGE: "
  local color_="\033[1;35m"
  local reset_="\033[0m"
  if [[ -t 1 ]]
  then
    echo -e "${color_}${prefix}${reset_}${text_}"
#     echo -e "\n${color_}${marker_}${reset_}\n${text_}\n${color_}${marker_}${reset_}\n"
  else
    echo -e "${prefix}${text_}"
#     echo -e "\n${marker_}\n${text_}\n${marker_}\n"
  fi
}

function header()
{
  local text_="$*"
  local spacer_="${text_//[[:print:]]/#}"
  local color_="\033[1;34m"
  local reset_="\033[0m"
  if [[ -t 1 ]]
  then
    echo -e "
${color_}######${spacer_}######${reset_}
${color_}#${reset_}     ${text_}     ${color_}#${reset_}
${color_}######${spacer_}######${reset_}"
  else
    echo "
######${spacer_}######
#     ${text_}     #
######${spacer_}######"
  fi
}

function usage()
{
  cat << HELP
USAGE

  ${0##*/} [options] <target> [<target>...]

TARGETS

  The special target "all" will build all components of Hipe External in order.
  Individual components can be build or rebuilt explicitly:

    * ffmpeg
    * livemedia
    * cuda8
    * opencv
    * dlib
    * boost


OPTIONS

  -a <archive dir path>
    Set the path to the source archive directory.
    Default: $ARCHIVE_DIRECTORY

  -b <build dir path>
    Set the path to the build directory.
    Default: $BUILD_DIRECTORY

  -c
    Build applications with CUDA support. This will use CUDA in the install
    directory if it is present, otherwise it will try to find CUDA on the
    system. If neither are available, the build will fail.

    This will also enable the cuda8 target when building "all".

  -d
    Clean up build files after each build. This can be used to free space when
    compiling all of Hipe external. The files are only removed after successful
    installation in the install directory.

  -g <git dir path>
    Set the path to the directory with the Git repositories.
    Default: $GIT_DIRECTORY

  -i <install dir path>
    Set the path to the installation directory.
    Default: $INSTALL_DIRECTORY

  -n <int>
    Set the number of build threads to pass to "make" with the "-j" option.
    Default: $N_THREADS (nproc + 1)

  -t <path>
    Set the temporary directory for CUDA builds. Make sure that this directory
    hand hold several GBs of temporary files. Because of this requirement, it
    is not set to /tmp by default, which may be a tmpfs system with limited
    storage capacity.
HELP

  if [[ ${1:-h} == h ]]
  then
    exit 0
  else
    exit 1
  fi
}

while getopts 'a:b:cdg:hi:n:t:' flag
do
  case "$flag" in
    a) ARCHIVE_DIRECTORY=$(readlink -f "$OPTARG") ;;
    b) BUILD_DIRECTORY=$(readlink -f "$OPTARG") ;;
    c) OPENCV_CUDA=ON ;;
    d) DELETE_BUILD_FILES=true ;;
    g) GIT_DIRECTORY=$(readlink -f "$OPTARG") ;;
    i) INSTALL_DIRECTORY=$(readlink -f "$OPTARG") ;;
    n) N_THREADS="$OPTARG" ;;
    t) TMPDIR=$(readlink -f "$OPTARG") ;;
    *) usage "$flag" ;;
  esac
done
shift $((OPTIND - 1))


message "ARCHIVE_DIRECTORY: $ARCHIVE_DIRECTORY"
message "BUILD_DIRECTORY:   $BUILD_DIRECTORY"
message "GIT_DIRECTORY: $GIT_DIRECTORY"
message "INSTALL_DIRECTORY: $INSTALL_DIRECTORY"
message "OPENCV_CUDA: $OPENCV_CUDA"
message "TMPDIR: $TMPDIR"

mkdir -p "$BUILD_DIRECTORY" "$INSTALL_DIRECTORY" "$TMPDIR"
export TMPDIR

# usage: prepend_pathlike <variable name> <path to prepend> [<separator>]
function prepend_pathlike()
{
  declare -n var_name="$1"
  local value="$2"
  local sep="${3:-:}"

  # If variable is empty or unset, set it to the argument.
  if [[ -z ${var_name-} ]]
  then
    var_name="$value"
  # Else prepend it with the argument and the separator if the value is not
  # already present.
  elif [[ ${sep}${var_name}${sep} != *${sep}${value}${sep}* ]]
  then
    var_name="${value}${sep}${var_name}"
  fi
  message "$1: $var_name"
}

function add_paths()
{
  local root_dir="$1"
  prepend_pathlike PATH "$root_dir/bin"
  prepend_pathlike PATH "$root_dir/usr/bin"
  prepend_pathlike LD_LIBRARY_PATH "$root_dir/lib"
  prepend_pathlike LD_LIBRARY_PATH "$root_dir/lib64"
  prepend_pathlike LD_LIBRARY_PATH "$root_dir/usr/lib"
  prepend_pathlike LD_LIBRARY_PATH "$root_dir/usr/lib64"
  prepend_pathlike PKG_CONFIG_PATH "$root_dir/lib/pkgconfig"
  prepend_pathlike PKG_CONFIG_PATH "$root_dir/usr/lib/pkgconfig"
  export PATH
  export LD_LIBRARY_PATH
  export PKG_CONFIG_PATH
}

# bsdtar handles all common archive formats
# untar <archive path>
function untar()
{
  message "extracting $1 to $BUILD_DIRECTORY"
  bsdtar -vC "$BUILD_DIRECTORY" -xf "$@"
}

# export master to the build directory
function export_git()
{
  local name_="${1##*/}"
  name_="${name_%.git}"

  local treeish_="${2:-master}"

  local build_dir="$BUILD_DIRECTORY/$name_"
  message "exporting $1 to $build_dir"
  mkdir -p "$build_dir"
  git -C "$1" archive --format=tar "$treeish_" | bsdtar -x -C "$build_dir" -f -
}



# Invoke make with options to use a specific GCC compiler.
function make_with_gcc_x()
{
  required_gcc_ver="$1"
  shift
  gcc_version="$(gcc -dumpversion)"
  if [[ ${gcc_version%%.*} != $required_gcc_ver ]]
  then
    local gcc_path="$(command -v gcc-$required_gcc_ver)"
    local gxx_path="$(command -v g++-$required_gcc_ver)"
    # This is necessary for poorly-coded build systems such as intel-tbb which
    # directly invokes "gcc" via the shell to determine the version.
    local tmp_dir="$(mktemp -p /tmp -d hipe_external_tmpbin_XXXXXX)"
    trap "rm -fr '$tmp_dir'" SIGINT SIGKILL
    ln -s "$gcc_path" "$tmp_dir/gcc"
    ln -s "$gxx_path" "$tmp_dir/g++"
    message "running make with gcc $required_gcc_ver ($gcc_path and $gxx_path, symlinks in $tmp_dir)"
    CC="$gcc_path" CPP="$gxx_path" CXX="$gxx_path" LD="$gxx_path" PATH="$tmp_dir:$PATH" make "$@"
    rm -fr "$tmp_dir"
  else
    make "$@"
  fi
}



# Invoke CMake with options to use a specific GCC compiler. This sets the gcc
# compiler used by nvcc as well.
function cmake_with_gcc_x()
{
  required_gcc_ver="$1"
  shift
  gcc_version="$(gcc -dumpversion)"
  if [[ ${gcc_version%%.*} != $required_gcc_ver ]]
  then
    local gcc_path="$(command -v gcc-$required_gcc_ver)"
    local gxx_path="$(command -v g++-$required_gcc_ver)"
    message "running CMake with gcc $required_gcc_ver ($gcc_path and $gxx_path)"
    CC="$gcc_path" CPP="$gxx_path" CXX="$gxx_path" LD="$gxx_path" cmake \
      -DCMAKE_C_COMPILER="$gcc_path" \
      -DCMAKE_CXX_COMPILER="$gxx_path" \
      "$@"
#       -DCUDA_NVCC_FLAGS='--expt-relaxed-constexpr' \
      # The gcc version is set in the nvcc script that wraps the nvcc binary in
      # the CUDA 8 installation directory. Some CMake/Make files fail to pass
      # the argument to all invocations of nvcc so a wrapper script was
      # necessary at the time of writing to ensure that the required version of
      # gcc is used.
#       -DCUDA_NVCC_FLAGS="-ccbin gcc-$required_gcc_ver" \
  else
    cmake "$@"
  fi
}



function maybe_remove_build_files()
{
  local build_dir="$BUILD_DIRECTORY/${1##*/}"
  if $DELETE_BUILD_FILES && [[ -e $build_dir ]]
  then
    message "removing $build_dir"
    rm -fr "$build_dir"
  fi
}



build_ffmpeg_deps()
{
  local install_dir="$INSTALL_DIRECTORY/ffmpeg"
  add_paths "$install_dir"

#   #yasm-1.3.0 (deprecated, use nasm below with current versions of x264)
#   message 'building yasm'
#   untar "$ARCHIVE_DIRECTORY/yasm-1.3.0.tar.gz"
#   pushd "$BUILD_DIRECTORY/yasm-1.3.0"
  #   ./configure --prefix="$install_dir" --bindir="$install_dir/bin"
  #   make -j "$N_THREADS" install
#   popd
#   maybe_remove_build_files "yasm-1.3.0"

  #yasm-2.13.02
  header 'building nasm'
  untar "$ARCHIVE_DIRECTORY/nasm-2.13.02.tar.gz"
  pushd "$BUILD_DIRECTORY/nasm-2.13.02"
    ./configure --prefix="$install_dir" --bindir="$install_dir/bin"
    make -j "$N_THREADS" install
  popd
  maybe_remove_build_files "nasm-2.13.02"

  #x264
  header 'building x264'
  # Export commit just before the x264_bit_depths global variable was dropped.
  # Required for compatibility with FFmpeg 3.3.2
  export_git "$GIT_DIRECTORY/x264" '2451a7282463f68e532f2eee090a70ab139bb3e7'
  pushd "$BUILD_DIRECTORY/x264"
    ./configure \
      --enable-debug \
      --disable-lavf \
      --prefix="$install_dir" \
      --bindir="$install_dir/bin" \
      --enable-static \
      --enable-shared \
      --disable-opencl
    make -j "$N_THREADS" install
  popd
  maybe_remove_build_files "x264"

  #x265
  #untar "$ARCHIVE_DIRECTORY/x265.tgz"
  #pushd $BUILD_DIRECTORY/x265/build/linux/
    #cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="$install_dir" -DENABLE_SHARED:bool=on -DENABLE_STATIC:bool=off ../../source
    #make -j "$N_THREADS" install
  #popd

  #fdk-aac
  header 'building fdk-aac'
  export_git "$GIT_DIRECTORY/fdk-aac"
  pushd "$BUILD_DIRECTORY/fdk-aac"
    autoreconf -fiv
    ./configure --enable-debug --prefix="$install_dir"
    make -j "$N_THREADS" install
  popd
  maybe_remove_build_files "fdk-aac"

  #lame-3.99.5
  #untar "$ARCHIVE_DIRECTORY/lame-3.99.5.tar.gz"
  #pushd $BUILD_DIRECTORY/lame-3.99.5
  #./configure --prefix="$install_dir" --enable-asm
  #make -j "$N_THREADS" install
  #popd

  #opus-1.1.4.tar.gz
  header 'building opus'
  untar "$ARCHIVE_DIRECTORY/opus-1.1.4.tar.gz"
  pushd "$BUILD_DIRECTORY/opus-1.1.4"
    ./configure --enable-debug --prefix="$install_dir"
    make -j "$N_THREADS" install
  popd
  maybe_remove_build_files "opus-1.1.4"

  #libvpx-1.6.1.tar.bz2
  header 'building libvpx'
  export_git "$GIT_DIRECTORY/libvpx" 'v1.6.1'
  pushd "$BUILD_DIRECTORY/libvpx"
    ./configure \
      --prefix="$install_dir" \
      --enable-runtime-cpu-detect \
      --enable-shared \
      --enable-pic \
      --disable-install-docs \
      --disable-install-srcs \
      --disable-examples \
      --disable-unit-tests \
      --enable-vp8 \
      --enable-postproc \
      --enable-vp9 \
      --enable-vp9-highbitdepth \
      --enable-experimental \
      --enable-spatial-svc
    make -j "$N_THREADS" install
  popd
  maybe_remove_build_files "libvpx"
}

function build_ffmpeg()
{
  #ffmpeg-3.2.2.tgz
  header 'building ffmpeg'

  local install_dir="$INSTALL_DIRECTORY/ffmpeg"
  add_paths "$install_dir"

  export_git "$GIT_DIRECTORY/ffmpeg" 'n3.3.2'
  pushd "$BUILD_DIRECTORY/ffmpeg"
#   untar "$ARCHIVE_DIRECTORY/ffmpeg-3.2.2.tar.gz"
#   pushd "$BUILD_DIRECTORY/ffmpeg-3.2.2"
    ./configure \
      --prefix="$install_dir" \
      --pkg-config-flags="--static" \
      --extra-cflags="-I$install_dir/include" \
      --extra-ldflags="-L$install_dir/lib" \
      --bindir="$install_dir/bin" \
      --enable-gpl \
      --enable-libfdk-aac \
      --enable-libfreetype \
      --enable-libopus \
      --enable-libtheora \
      --enable-libvorbis \
      --enable-libvpx \
      --enable-libx264 \
      --enable-nonfree \
      --enable-shared \
      --enable-debug

    make -j "$N_THREADS" install
#     hash -r
  popd
  maybe_remove_build_files "ffmpeg"
}


function build_livemedia()
{
  #live-media 2017.10.28
  header 'building live-media'

  local install_dir="$INSTALL_DIRECTORY/livemedia"
  add_paths "$install_dir"

  untar "$ARCHIVE_DIRECTORY/live555-latest.tar.gz" -s '/live/live/'
  pushd "$BUILD_DIRECTORY/live"
    local glibc_version="$(LC_ALL=C ldd --version | awk 'NR==1{print $NF}')"
    local glibc_maj_ver="${glibc_version%.*}"
    local glibc_min_ver="${glibc_version#*.}"
    if [[ $glibc_maj_ver -gt 2  || ($glibc_maj_ver == 2 && $glibc_min_ver -ge 26) ]]
    then
      sed -e '/xlocale.h/d' -i liveMedia/include/Locale.hh # required for glibc 2.26
    fi
    sed \
      -e 's/$(INCLUDES) -I. -O2 -DSOCKLEN_T/$(INCLUDES) -I. -O2 -I. -fPIC -DRTSPCLIENT_SYNCHRONOUS_INTERFACE=1 -DSOCKLEN_T/g' \
      -i config.linux
    ./genMakefiles linux
    make -j "$N_THREADS" PREFIX="" DESTDIR="$install_dir" install
  popd
  maybe_remove_build_files "livemedia"
}


function build_cuda8()
{
  header 'building CUDA 8'
  local install_dir="$INSTALL_DIRECTORY/cuda8"
  add_paths "$install_dir"
  "$script_dir"/install_cuda_8.sh "$install_dir"
}


function build_caffe()
{
  header ='building caffe'
  local install_dir="$INSTALL_DIRECTORY/caffe"
  export_git "$GIT_DIRECTORY/caffe" '1.0'
  local build_dir="$BUILD_DIRECTORY/caffe/build"
  mkdir -p -- "$build_dir"
  pushd -- "$build_dir"
    cmake_with_gcc_x 5 \
      -DCMAKE_INSTALL_PREFIX="$install_dir" \
      -DWITH_CUDA=OFF \
      ..
      -DWITH_CUDA="$OPENCV_CUDA" \
    make -j "$N_THREADS" install
  popd
  maybe_remove_build_files "caffe"
}



function build_opencv()
{
  header 'building opencv with tbb'

#   if [[ -d $INSTALL_DIRECTORY/boost ]]
#   then
#     local boost_install_dir="$INSTALL_DIRECTORY/boost"
#     add_paths "$boost_install_dir"
#   fi

  if [[ $OPENCV_CUDA == ON && -d $INSTALL_DIRECTORY/cuda8 ]]
  then
    local cuda_install_dir="$INSTALL_DIRECTORY/cuda8"
    add_paths "$cuda_install_dir"
  fi

  local install_dir="$INSTALL_DIRECTORY/opencv"

  add_paths "$install_dir"

  # Required from sfm module in opencv contrib.
#   header 'building ceres-solver'
#   export_git "$GIT_DIRECTORY/ceres-solver" '1.12.0'
#   mkdir -p -- "$BUILD_DIRECTORY/ceres-solver/build"
#   pushd -- "$BUILD_DIRECTORY/ceres-solver/build"
#     cmake -DLIB_SUFFIX='' ../
#     make -j "$N_THREADS" DESTDIR="$install_dir" install
#   popd
#   maybe_remove_build_files "ceres-solver"

  if [[ -e $ARCHIVE_DIRECTORY/intel-tbb-2018_U2.tar.gz ]]
  then
    header 'building tbb'
    untar "$ARCHIVE_DIRECTORY/intel-tbb-2018_U2.tar.gz"
    pushd "$BUILD_DIRECTORY/tbb-2018_U2"
      CXXFLAGS="${CXXFLAGS:-} -fno-lifetime-dse" make_with_gcc_x 5
  #     make_with_gcc_x 5
      install -d "$install_dir"/usr/lib
      install -m755 build/linux_*/*.so* "$install_dir"/usr/lib
      install -d "$install_dir"/usr/include
      cp -a include/tbb "$install_dir"/usr/include
    popd
  fi
  maybe_remove_build_files "tbb-2018_U2"


  header 'building opencv'
  untar "$ARCHIVE_DIRECTORY/opencv_contrib-3.4.0.zip"
  untar "$ARCHIVE_DIRECTORY/opencv-3.4.0.zip"

  # Disable detection of ceres-solver.
  sed -i 's/find_package(Ceres QUIET)/set(Ceres_FOUND FALSE)/' \
    "$BUILD_DIRECTORY/opencv_contrib-3.4.0/modules/sfm/CMakeLists.txt"

  # ensure clean build directory
  local build_dir="$BUILD_DIRECTORY/opencv-3.4.0/build"
#   rm -rf -- "$build_dir"
  mkdir -p -- "$build_dir"
  pushd -- "$build_dir"
    cmake_with_gcc_x 5 \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX="$install_dir" \
      -DOPENCV_EXTRA_MODULES_PATH="$BUILD_DIRECTORY/opencv_contrib-3.4.0/modules" \
      -DCMAKE_DISABLE_FIND_PACKAGE_Caffe=TRUE \
      -DWITH_CUDA="$OPENCV_CUDA" \
      -DWITH_GSTREAMER=ON \
      -DENABLE_FAST_MATH=ON \
      -DCUDA_FAST_MATH=ON \
      -DWITH_CUBLAS=ON \
      -DBUILD_opencv_python=ON \
      -DBUILD_opencv_python2=ON \
      -DBUILD_opencv_python3=OFF \
      -DENABLE_PRECOMPILED_HEADERS=OFF \
      ..
#       -DBUILD_PROTOBUF=OFF \
    make -j "$N_THREADS" install
  popd
  maybe_remove_build_files "opencv-3.4.0"
  maybe_remove_build_files "opencv_contrib-3.4.0"
}




function build_dlib()
{
  #dlib 19.13
  header 'building dlib'

#   if [[ $OPENCV_CUDA == ON && -d $INSTALL_DIRECTORY/cuda8 ]]
#   then
#     local cuda_install_dir="$INSTALL_DIRECTORY/cuda8"
#     add_paths "$cuda_install_dir"
#   fi

  local install_dir="$INSTALL_DIRECTORY/dlib"
  add_paths "$install_dir"

  untar "$ARCHIVE_DIRECTORY/dlib-19.13.zip"
  mkdir -p -- "$BUILD_DIRECTORY/dlib-19.13/build"
  pushd "$BUILD_DIRECTORY/dlib-19.13/build"
  cmake \
    -DCMAKE_INSTALL_PREFIX="$install_dir" \
    ..
  make -j "$N_THREADS" install
#   make -j "$N_THREADS" DESTDIR="$install_dir" install
  popd
  mkdir -p -- "$BUILD_DIRECTORY/dlib-19.13/build2"  
  pushd "$BUILD_DIRECTORY/dlib-19.13/build2"
  cmake \
      -DCMAKE_INSTALL_PREFIX="$install_dir" \
	  -DBUILD_SHARED_LIBS=ON \
    ..
  make -j "$N_THREADS" install
#   make -j "$N_THREADS" DESTDIR="$install_dir" install
  popd

  maybe_remove_build_files "dlib"
}


function build_boost()
{
  header "building Boost"

  local install_dir="$INSTALL_DIRECTORY/boost"
  add_paths "$install_dir"

  # Get the path to the Python 2 executable and the version. "2>&1" redirects
  # STDERR to STDOUT so that it is piped to sed. Python 3 writes its version to
  # STDOUT while Python 2 writes to stderr.
  if command -v python
  then
    pypath=$(command -v python)
    pyver="$("$pypath" -V 2>&1 | sed 's@Python \([0-9]\+\.[0-9]\+\).*@\1@')"
    message  "Python path: $pypath\nPython version: $pyver"
  else
    pyver=''
    message 'No "python" executable found.'
  fi
  if [[ ${pyver:0:1} != 2 ]]
  then
    message 'Searching for python2*.'
    local unchecked_path="$PATH"
    while [[ ${pyver:0:1} != 2 && ! -z $unchecked_path ]]
    do
      local this_path="${unchecked_path%%:*}"
      local next_path_index=$((${#this_path} + 1))
      local unchecked_path="${unchecked_path:${next_path_index}}"
      local python2_candidate
      for pypath in "$this_path/python2"*
      do
        pyver="$("$pypath" -V 2>&1 | sed 's@Python \([0-9]\+\.[0-9]\+\).*@\1@')"
        [[ ${pyver:0:1} == 2 ]] && break
      done
    done
  fi

  if [[ ${pyver:0:1} != 2 ]]
  then
    message "error: failed to find Python2 executable"
    exit 1
  else
    message  "Python2 path: $pypath\nPython version: $pyver"
  fi

  untar "$ARCHIVE_DIRECTORY/boost_1_62_0.tar.gz"
  pushd "$BUILD_DIRECTORY/boost_1_62_0"
  ./bootstrap.sh --prefix="$install_dir" --with-toolset=gcc --with-icu --with-python="$pypath"
    # Support for OpenMPI
    echo "using mpi ;" >> project-config.jam

    #linkflags="${LDFLAGS:-}"
    ./b2 \
      variant=release \
      debug-symbols=off \
      threading=multi \
      runtime-link=shared \
      toolset=gcc \
      python="$pyver" \
      cflags="${CPPFLAGS:-} ${CFLAGS:-} -fPIC -O3" \
      cxxflags="${CPPFLAGS:-} ${CXXFLAGS:-} -std=c++11 -fPIC -O3" \
      --layout=system \
      --prefix="$install_dir" \
      install
  popd
  maybe_remove_build_files "boost_1_62_0"
}



build_python27()
{
  header "building Python 2.7"

  local install_dir="$INSTALL_DIRECTORY/python27"
  add_paths "$install_dir"
  untar "$ARCHIVE_DIRECTORY/Python-2.7.14.tgz"
  pushd "$BUILD_DIRECTORY/Python-2.7.14"

    local pkgver=2.7.14
    local _pybasever=2.7

    # Follow most of Arch Linux's build procedure:
    # https://www.archlinux.org/packages/extra/x86_64/python2/

    # Temporary workaround for FS#22322
    # See http://bugs.python.org/issue10835 for upstream report
    sed -i "/progname =/s/python/python${_pybasever}/" Python/pythonrun.c

    # Enable built-in SQLite module to load extensions (fix FS#22122)
    sed -i "/SQLITE_OMIT_LOAD_EXTENSION/d" setup.py

    # FS#23997
    sed -i -e "s|^#.* /usr/local/bin/python|#!/usr/bin/python2|" Lib/cgi.py

    sed -i "s/python2.3/python2/g" Lib/distutils/tests/test_build_scripts.py \
      Lib/distutils/tests/test_install_scripts.py

    # clean up #!s
    find . -name '*.py' | \
      xargs sed -i "s|#[ ]*![ ]*/usr/bin/env python$|#!/usr/bin/env python2|"

    # Workaround asdl_c.py/makeopcodetargets.py errors after we touched the shebangs
    touch Include/Python-ast.h Python/Python-ast.c Python/opcode_targets.h

    # FS#48761
    # http://bugs.python.org/issue25750
    patch -Np1 -i "$PATCH_DIRECTORY"/python27-descr_ref.patch

    # Backport from Python 3.x
    patch -Np2 -i "$PATCH_DIRECTORY"/python27-fix-profile-task.patch

    OPT="${CFLAGS:-}" ./configure --prefix=/usr \
                --enable-shared \
                --with-threads \
                --enable-optimizations \
                --with-lto \
                --enable-ipv6 \
                --enable-unicode=ucs4 \
                --with-system-expat \
                --with-system-ffi \
                --with-dbmliborder=gdbm:ndbm \
                --without-ensurepip

    make DESTDIR="${install_dir}" altinstall maninstall

    rm "${install_dir}"/usr/share/man/man1/python.1

    ln -sf python${_pybasever}        "${install_dir}"/usr/bin/python2
    ln -sf python${_pybasever}-config "${install_dir}"/usr/bin/python2-config
    ln -sf python${_pybasever}.1      "${install_dir}"/usr/share/man/man1/python2.1

    # FS#33954
    ln -sf python-${_pybasever}.pc    "${install_dir}"/usr/lib/pkgconfig/python2.pc

    ln -sf ../../libpython${_pybasever}.so "${install_dir}"/usr/lib/python${_pybasever}/config/libpython${_pybasever}.so

    mv "${install_dir}"/usr/bin/smtpd.py "${install_dir}"/usr/lib/python${_pybasever}/

    # some useful "stuff"
    install -dm755 "${install_dir}"/usr/lib/python${_pybasever}/Tools/{i18n,scripts}
    install -m755 Tools/i18n/{msgfmt,pygettext}.py "${install_dir}"/usr/lib/python${_pybasever}/Tools/i18n/
    install -m755 Tools/scripts/{README,*py} "${install_dir}"/usr/lib/python${_pybasever}/Tools/scripts/

    # fix conflicts with python
    mv "${install_dir}"/usr/bin/idle{,2}
    mv "${install_dir}"/usr/bin/pydoc{,2}
    mv "${install_dir}"/usr/bin/2to3{,-2.7}

    # clean-up reference to build directory
    sed -i "s#${PWD}/Python-${pkgver}:##" "${install_dir}"/usr/lib/python${_pybasever}/config/Makefile

    # license
    install -Dm644 LICENSE "${install_dir}"/usr/share/licenses/Python27/LICENSE

  popd


  untar "$ARCHIVE_DIRECTORY/Numpy-v1.14.0.tar.gz"
  pushd "$BUILD_DIRECTORY/numpy-1.14.0"
    python2 setup.py config_fc --fcompiler=gnu95 install --prefix=/usr --root="${install_dir}" --optimize=1
  popd
}





for target_ in "$@"
do
  case "${target_,,}" in
    all)
#       build_python27
      build_ffmpeg_deps
      build_ffmpeg
      build_livemedia
      build_boost
      build_dlib
      [[ $OPENCV_CUDA == ON ]] && build_cuda8
      build_opencv
    ;;

    python27)
      build_python27
    ;;

    ffmpeg)
      build_ffmpeg_deps
      build_ffmpeg
    ;;

    livemedia)
      build_livemedia
    ;;

    cuda8)
      build_cuda8
    ;;

    opencv)
      build_opencv
    ;;

    dlib)
      build_dlib
    ;;

    boost)
      build_boost
    ;;

  esac
done
