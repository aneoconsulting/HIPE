#!/bin/bash

# README
# See the separate README for troubleshooting suggestions.



# Exit on error or use of unset variable.
set -eu
shopt -s nullglob

self_="$(readlink -f "$0")"
script_dir="${self_%/*}"

ARCHIVE_DIRECTORY="$script_dir/downloads/archives"
BUILD_DIRECTORY="$script_dir/build"
INSTALL_DIRECTORY="$script_dir/install"
GIT_DIRECTORY="$script_dir/downloads"
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

  ${0##*/} [options] 

OPTIONS

  -a <archive dir path>
    Set the path to the source archive directory.
    Default: $ARCHIVE_DIRECTORY

  -b <build dir path>
    Set the path to the build directory.
    Default: $BUILD_DIRECTORY

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



function build_glog()
{
    add_paths "$INSTALL_DIRECTORY"


	pushd "$BUILD_DIRECTORY"

	git clone https://github.com/BVLC/caffe.git
	
    sudo apt-get install libprotobuf-dev libleveldb-dev libsnappy-dev libhdf5-serial-dev protobuf-compiler libopenblas-dev python-dev
	
	pushd "$BUILD_DIRECTORY/caffe/build"
		cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$INSTALL_DIRECTORY" ..
		make -j $N_THREADS install
	popd #"$BUILD_DIRECTORY/boringssl/build"

	
	popd #$BUILD_DIRECTORY/
}


build_glog
