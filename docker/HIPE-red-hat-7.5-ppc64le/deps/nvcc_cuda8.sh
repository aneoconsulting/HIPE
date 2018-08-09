#!/bin/bash
self_="$(readlink -f "$0")"
bin_dir="${self_%/*}"

nvcc_cmd=("$bin_dir/nvcc.orig")

function messagerun()
{
#   local text_="${@@Q}"
  local text_="$@"
  if command -v tput >/dev/null
  then
    local width_=$(tput cols)
    local marker_="$(printf "%${width_}s")"
    marker_="${marker_// /#}"
  else
    local marker_='####################'
  fi
  local color_="\033[1;35m"
  local reset_="\033[0m"
  if [[ -t 2 ]]
  then
    echo -e "\n${color_}${marker_}${reset_}\n${text_}\n${color_}${marker_}${reset_}\n" >&2
  else
    echo -e "\n${marker_}\n${text_}\n${marker_}\n" >&2
  fi
  "$@"
}

# Check for existing arguments.
has_ccbin=false
has_std=false
has_relaxed_constexpr=false
for arg_ in "$@"
do
  case "$arg_" in
    -ccbin) has_ccbin=true ;;
    -std=*) has_std=true ;;
    --expt-relaxed-constexpr) has_relaxed_constexpr=true ;;
  esac
done

if ! $has_ccbin
then
  required_gcc_ver=5
  gcc_version="$(gcc -dumpversion)"
  if [[ ${gcc_version%%.*} != $required_gcc_ver ]]
  then
    cc_cmd="$(command -v gcc-$required_gcc_ver)"
    nvcc_cmd+=(-ccbin "$cc_cmd")
  fi
fi

if ! $has_std
then
  nvcc_cmd+=(-std=c++11)
fi

if ! $has_relaxed_constexpr
then
  nvcc_cmd+=(--expt-relaxed-constexpr)
fi

messagerun "${nvcc_cmd[@]}" "$@"
