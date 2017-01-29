#!/bin/bash

set -eu
set -o pipefail

CLANG_VERSION="${1}"
mason install clang++ ${CLANG_VERSION}
export PATH=$(mason prefix clang++ ${CLANG_VERSION})/bin:${PATH}
export CXX=$(mason prefix clang++ ${CLANG_VERSION})/bin/clang++
