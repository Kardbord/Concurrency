#!/usr/bin/env bash
set -e

function usage() {
  echo "${0}"
  echo
  echo "Usage"
  echo "  ${0} [-h] [-p]"
  echo
  echo "Options"
  echo "  -h                Prints this usage"
  echo "  -p INSTALL_PREFIX Sets the target path for installation. If unspecified, uses the system default."
}

INSTALL_PREFIX=""
while getopts ":hp:" opt; do
  case "${opt}" in
  p)
    INSTALL_PREFIX="${OPTARG}"
    ;;
  h)
    usage
    exit 0
    ;;
  *)
    usage
    exit 1
    ;;
  esac
done
shift $((OPTIND - 1))

if [[ -n "${INSTALL_PREFIX}" ]] && ! [[ -d "${INSTALL_PREFIX}" ]]; then
  echo "No such directory: ${INSTALL_PREFIX}"
  exit 1
fi

mkdir -p "$(dirname "${BASH_SOURCE[0]}")/../build"
pushd "$(dirname "${BASH_SOURCE[0]}")/../build" >/dev/null
rm -rf ./* # Start from clean slate
cmake .. "-DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_PREFIX}" -DINSTALL=ON
cmake --build . --config Release --target install -- -j "$(nproc)"
rm -rf ./* # Clean up build files
popd >/dev/null
