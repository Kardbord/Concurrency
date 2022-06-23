#!/usr/bin/env bash

SELF=$(readlink -f "${0}")
PROJECT_NAME=
LICENSE=

function usage() {
  echo "${0}"
  echo
  echo "USAGE"
  echo "${0} -n PROJECT_NAME [-l LICENSE]"
  echo
  echo "This script performs project initialization. Specifically,"
  echo "it removes any temporary template files, sets the project name,"
  echo "optionally creates a LICENSE file, then deletes itself."
  echo
  echo "OPTIONS"
  echo "-n PROJECT_NAME"
  echo "-l LICENSE"
}

function license() {
  [ -z "${LICENSE}" ] && return 0
  echo "-l is not yet supported. :("
  echo "Try again without setting a license."
  exit 2
}

function name() {
  sed -i "s|^project(.*)$|project(${PROJECT_NAME})|g" CMakeLists.txt
  sed -i '1,/^---$/d' README.md
  sed -i "s|Project Title|${PROJECT_NAME}|g" README.md
  sed -i "s|Project description\.|${PROJECT_NAME}|g" README.md

}

function clean() {
  find . -name 'README.md' -not -path "./.git/*" -not -path "./build/*" -not -wholename "./README.md" -exec rm -f {} \;
  rm -f "${SELF}"
}

function process_args() {
  while getopts ":n:l:" opt; do
    case "${opt}" in
      n)
        PROJECT_NAME="${OPTARG}"
        ;;
      l)
        LICENSE="${OPTARG}"
        ;;
      h)
        usage
        exit 0
        ;;
      *)
        usage
        exit 1
    esac
  done
  shift $((OPTIND-1))
}

function main() {
  set -e
  pushd $(dirname "${BASH_SOURCE[0]}")/.. > /dev/null

  process_args "$@"
  if [ -z "${PROJECT_NAME}" ]; then
    usage
    exit 0
  fi
  
  license
  name
  clean
}

main "$@"
