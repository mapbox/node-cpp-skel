#!/bin/bash

set -eu
set -o pipefail

if [[ $(uname -s) == 'Linux' ]]; then
    mason install clang++ 3.9.0
    export PATH=$(mason prefix clang++ 3.9.0)/bin:${PATH}
    export CXX=$(mason prefix clang++ 3.9.0)/bin/clang++
fi
