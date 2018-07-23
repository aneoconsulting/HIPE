#!/bin/bash
set -eu

if [[ $# -lt 1 ]]
then
  echo "usage: ${0##*/} <installation directory>" >&2
  exit 1
fi

self_="$(readlink -f "$0")"
self_dir="${self_%/*}"
cuda_archive_dir="${self_dir}/downloads/archives"
extraction_dir="${self_dir}/build/cuda8"
install_dir="$(readlink -f "$1")"

mkdir -p "$extraction_dir" "$install_dir"

if [[ ! -e $extraction_dir/cuda-linux64-rel-8.0.61-21551265.run ]]
then
  echo "extracting cuda runtime..."
  bash "$cuda_archive_dir"/cuda_8.0.61_375.26_linux.run --extract "$extraction_dir"
fi
cd "$extraction_dir"

if [[ ! -e pkg ]]
then
  ./cuda-linux64-rel-8.0.61-21551265.run --noexec --keep

  pypath=$(command -v python)
  pyver="$("$pypath" -V 2>&1 | sed 's@Python \([0-9]\+\.[0-9]\+\).*@\1@')"
  echo  "System Python path: $pypath\nPython version: $pyver"
  if [[ ${pyver:0:1} != 2 ]]
  then
    pypath=$(command -v python2)
    pyver="$("$pypath" -V 2>&1 | sed 's@Python \([0-9]\+\.[0-9]\+\).*@\1@')"
    echo  "Updating to use Python path: $pypath\nPython version: $pyver"
    find pkg -name '*.py' -exec sed -i -e 's|env python.*|env python2|g' -e 's|bin/python.*|bin/python2|g' '{}' \+
  fi
fi

cd 'pkg'

export PERL5LIB=.
echo "installing CUDA 8 to $install_dir"
perl install-linux.pl -prefix="$install_dir" -noprompt

bash  "$cuda_archive_dir"/cuda_8.0.61.2_linux.run --accept-eula --silent --installdir="$install_dir"
nvcc_path="$install_dir"/bin/nvcc
[[ -e $nvcc_path.orig ]] || mv "$nvcc_path"{,.orig}
install -Dm755 "$self_dir/nvcc_cuda8.sh" "$nvcc_path"
