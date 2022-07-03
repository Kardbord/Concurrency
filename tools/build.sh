#!/usr/bin/env bash

set -e
pushd "$(dirname "${BASH_SOURCE[0]}")/../build" > /dev/null
cmake .. && make
popd > /dev/null