#!/usr/bin/env bash

set -eu
set -o pipefail

export MASON_VERSION="0.5.0"
export CLANG_VERSION="3.9.1"

# unbreak bash shell due to rvm bug on osx: https://github.com/direnv/direnv/issues/210#issuecomment-203383459
# this impacts any usage of scripts that are source'd (like this one)
if [[ "${TRAVIS_OS_NAME}" == "osx" ]]; then
  echo 'shell_session_update() { :; }' > ~/.direnvrc
fi

#
# MASON
#

source ${CURRENT_DIR}/setup_mason.sh ${MASON_VERSION}

#
# COMPILER
#

source ${CURRENT_DIR}/setup_compiler.sh ${CLANG_VERSION}

# set back to non-strict bash mode to avoid breaking travis itself
set +eu
set +o pipefail