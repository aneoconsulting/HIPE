#!/bin/bash
set -eu

self_="$(readlink -f "$0")"
self_dir="${self_%/*}"
cuda_archive_dir="${self_dir}/downloads/"



if [ -d "/usr/local/cuda-10.0" ]; then
	echo "Cuda 10.0 Already installed. Done"
	exit 0
fi

sudo bash "${cuda_archive_dir}"/cuda_10.0.run -silent --tolkit


