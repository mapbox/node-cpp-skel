#!/usr/bin/env bash

set -eu
set -o pipefail

# https://clang.llvm.org/extra/clang-tidy/

: '
Runs clang-tidy on the code in src/
Return `1` if there are files automatically fixed by clang-tidy.
Returns `0` if no fixes by clang-tidy.
TODO: should also return non-zero if clang-tidy emits warnings
or errors about things it cannot automatically fix. However I cannot
figure out how to get this working yet as it seems that clang-tidy
always returns 0 even on errors.
'

PATH_TO_CLANG_TIDY_SCRIPT="$(pwd)/mason_packages/.link/share/run-clang-tidy.py"
# make sure that run-clang-tidy.py can find the right clang-tidy
export PATH=$(pwd)/mason_packages/.link/bin:${PATH}

# build the compile_commands.json file if it does not exist
if [[ ! -f build/compile_commands.json ]]; then
    # We need to clean otherwise when we make the project
    # will will not see all the compile commands
    make clean
    # Create the build directory to put the compile_commands in
    # We do this first to ensure it is there to start writing to
    # immediately (make make not create it right away)
    mkdir -p build
    # Run make, pipe the output to the generate_compile_commands.py
    # and drop them in a place that clang-tidy will automatically find them
    RESULT=0
    make > /tmp/make-node-cpp-skel-build-output.txt || RESULT=$?
    if [[ ${RESULT} != 0 ]]; then
        echo "Build failed, could not generate compile commands for clang-tidy, aborting!"
        exit ${RESULT}
    else
        cat /tmp/make-node-cpp-skel-build-output.txt | scripts/generate_compile_commands.py > build/compile_commands.json
    fi

fi

# change into the build directory so that clang-tidy can find the files
# at the right paths (since this is where the actual build happens)
cd build
${PATH_TO_CLANG_TIDY_SCRIPT} -fix
cd ../

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
