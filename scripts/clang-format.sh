#!/usr/bin/env bash

set -eu
set -o pipefail

: '

Runs clang-format on the code in src/

Return `1` if there are files to be formatted, and automatically formats them.

Returns `0` if everything looks properly formatted.

'

PATH_TO_FORMAT_SCRIPT="$(pwd)/mason_packages/.link/bin/clang-format"

# to speed up re-runs, only re-create environment if needed
if [[ ! -f local.env ]] || [[ ! -f ${PATH_TO_FORMAT_SCRIPT} ]]; then
    # automatically setup environment
    ./scripts/setup.sh --config local.env
fi

# source the environment
source local.env

# to speed up re-runs, only install clang-format if needed
if [[ ! -f ${PATH_TO_FORMAT_SCRIPT} ]]; then
    # The MASON_LLVM_RELEASE variable comes from `local.env`
    node_modules/.bin/mason-js install clang-format=${MASON_LLVM_RELEASE} --type=compiled
    node_modules/.bin/mason-js link clang-format=${MASON_LLVM_RELEASE} --type=compiled
    # We link the tools to make it easy to know ${PATH_TO_FORMAT_SCRIPT}
fi

# Run clang-format on all cpp and hpp files in the /src directory
find src/ -type f -name '*.hpp' -o -name '*.cpp' \
 | xargs -I{} ${PATH_TO_FORMAT_SCRIPT} -i -style=file {}

# Print list of modified files
dirty=$(git ls-files --modified src/)

if [[ $dirty ]]; then
    echo "The following files have been modified:"
    echo $dirty
    git diff
    exit 1
else
    exit 0
fi