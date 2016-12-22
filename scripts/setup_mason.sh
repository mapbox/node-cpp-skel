#!/bin/bash

set -eu
set -o pipefail

# we pin the mason version to avoid changes in mason breaking builds
MASON_VERSION=$1

function setup_mason() {
    if [[ ! -d ./.mason ]]; then
        git clone https://github.com/mapbox/mason.git ./.mason
        (cd ./.mason && git checkout ${MASON_VERSION})
    else
        echo "Updating to latest mason"
        (cd ./.mason && git fetch && git checkout ${MASON_VERSION})
    fi
    export PATH=$(pwd)/.mason:$PATH
}

setup_mason
