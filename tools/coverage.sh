#!/usr/bin/env bash

set -e
pushd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null
./build.sh -b Coverage
popd >/dev/null
pushd "$(dirname "${BASH_SOURCE[0]}")/../build/" >/dev/null
make concurrency_coverage
popd >/dev/null
echo "View coverage results by opening the html file referenced above in a browser."
