#!/usr/bin/env bash

set -eu
set -o pipefail

export TARGET=${TARGET:-Release}
export CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export NODE_VERSION=${NODE:-4}
export MASON_VERSION="master"
export NPM_FLAGS=${NPM_FLAGS:-}

#
# NODE
#

echo $NODE_VERSION
source ./scripts/setup_node.sh ${NODE_VERSION}

#
# MASON
#

source ${CURRENT_DIR}/setup_mason.sh ${MASON_VERSION}

#
# COMPILER
#

source ${CURRENT_DIR}/setup_compiler.sh

#
# INSTALL NODE DEPS
#

npm install --build-from-source ${NPM_FLAGS}

# set back to non-strict bash mode to avoid breaking travis itself
set +eu
set +o pipefail