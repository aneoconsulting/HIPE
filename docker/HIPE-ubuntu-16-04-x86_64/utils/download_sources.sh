#!/bin/bash
set -eu

PARALLEL=false
ARIA2C=false
CURL=false


function usage()
{
  cat << HELP
USAGE

  ${0##*/} [options]

OPTIONS

  -a
    Use Aria2 (aria2c) for parallel and segmented downloads of the archives.
    Without this option, wget will be used.

  -c
    Use curl to download the metalink.

  -p
    Download files in parallel. This will background each separate download
    command and then wait for all of them to finish. While this will normally
    shorten the download time, it has two disadvantages:

    * The output of all commands is printed simultaneously and overlaps in the
      console.
    * The processes must be killed explictly because <ctrl>+c will only kill the
      parent process without propagating the signal to the child processes.
HELP

  if [[ ${1:-h} == h ]]
  then
    exit 0
  else
    exit 1
  fi
}

while getopts 'acp' flag
do
  case "$flag" in
    a) ARIA2C=true ;;
    c) CURL=true ;;
    p) PARALLEL=true ;;
    *) usage "$flag" ;;
  esac
done
shift $((OPTIND - 1))



self_="$(readlink -f "$0")"
self_dir="${self_%/*}"
gen_dir="${self_dir}/gen"

metalink_path="${gen_dir}/archives.metalink"
wget_cmds_path="archives.wget.sh"
git_repos_list_path="git_repos.txt"

pids_=()
download_dir="$(readlink -f "${1:-downloads}")"

function runcmd()
{
  if $PARALLEL
  then
    "$@" &
    pids_+=($!)
  else
    "$@"
  fi
}

mkdir -p "$download_dir"/{archives,git}
cd -- "$download_dir"

pushd -- archives
  if $ARIA2C
  then
    runcmd aria2c --metalink-file "$metalink_path" --continue --allow-overwrite
  elif $CURL
  then
    curl --metalink "file://$metalink_path"
  else
    source -- "$wget_cmds_path"
  fi
popd

pushd -- git
#     'https://github.com/BVLC/caffe.git' \
#     'https://ceres-solver.googlesource.com/ceres-solver' \
  while read -r git_url
  do
    name_="${git_url##*/}" # basename
    name_="${name_%.git}" # strip ".git" suffix
    name_="${name_,,}" # make lower case
    if [[ -e $name_ ]]
    then
      pushd "$name_"
        runcmd git fetch
      popd
    else
      runcmd git clone --bare "$git_url" "$name_"
    fi
  done < "$git_repos_list_path"
popd

if $PARALLEL
then
  # Wait for processes to finish.
  for pid_ in "${pids_[@]}"
  do
    while [[ -e /proc/$pid_ ]]
    do
      sleep 0.5
    done
  done
fi
