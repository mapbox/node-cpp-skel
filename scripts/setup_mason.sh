#!/bin/bash

set -eu
set -o pipefail

MASON_RELEASE=$1

function setup_mason() {
 # install clang++ via mason
  rm -rf /tmp/mason-${MASON_RELEASE}
  mkdir /tmp/mason-${MASON_RELEASE}
  curl -sSfL https://github.com/mapbox/mason/archive/v${MASON_RELEASE}.tar.gz | tar --gunzip --extract --strip-components=1 --directory=/tmp/mason-${MASON_RELEASE}
  export PATH=/tmp/mason-${MASON_RELEASE}:${PATH}
}

setup_mason
