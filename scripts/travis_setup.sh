#!/usr/bin/env bash

set -eu
set -o pipefail

MASON_VERSION="master"

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

function init_binary() {
    mason install ${1} ${2}
    export PATH=$(mason prefix ${1} ${2})/bin:${PATH}

}

function main() {
    setup_mason
    if [[ $(uname -s) == 'Linux' ]]; then
        init_binary clang 3.8.0
        export CXX=clang++
        export CC=clang
    fi
    if [[ ${NODE_ASAN:-} == 'true' ]]; then
        init_binary node_asan 4.4.5
    fi
}

main

# set back to non-strict bash mode to avoid breaking travis itself
set +eu
set +o pipefail

