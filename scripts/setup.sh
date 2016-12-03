#!/usr/bin/env bash

set -eu
set -o pipefail

export TARGET=${TARGET:-Release}
export CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export MASON_VERSION="master"
export NPM_FLAGS=${NPM_FLAGS:-}

#
# MASON
#

source ${CURRENT_DIR}/setup_mason.sh ${MASON_VERSION}

#
# COMPILER
#

source ${CURRENT_DIR}/setup_compiler.sh

# set back to non-strict bash mode to avoid breaking travis itself
set +eu
set +o pipefail