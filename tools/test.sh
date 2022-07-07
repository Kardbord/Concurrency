#!/usr/bin/env bash

set -e
pushd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null
./build.sh
../build/ConcurrentMap_test
popd >/dev/null
