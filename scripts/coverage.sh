#!/usr/bin/env bash

set -eu
set -o pipefail

export CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source ${CURRENT_DIR}/setup_mason.sh master

# http://clang.llvm.org/docs/UsersManual.html#profiling-with-instrumentation
# https://www.bignerdranch.com/blog/weve-got-you-covered/

mason install clang++ 4.0.0
mason install llvm-cov 4.0.0

export CXX="$(mason prefix clang++ 4.0.0)/bin/clang++"
export PROF=$(mason prefix llvm-cov 4.0.0)/bin/llvm-profdata
export COV=$(mason prefix llvm-cov 4.0.0)/bin/llvm-cov
export CXXFLAGS="-fprofile-instr-generate -fcoverage-mapping"
export LDFLAGS="-fprofile-instr-generate"
make debug
rm -f *profraw
rm -f *gcov
rm -f *profdata
LLVM_PROFILE_FILE="code-%p.profraw" npm test
CXX_MODULE=$(./node_modules/.bin/node-pre-gyp reveal module --silent)
${PROF} merge -output=code.profdata code-*.profraw
${COV} report ${CXX_MODULE} -instr-profile=code.profdata -use-color
${COV} show ${CXX_MODULE} -instr-profile=code.profdata src/*.cpp -filename-equivalence -use-color
${COV} show ${CXX_MODULE} -instr-profile=code.profdata src/*.cpp -filename-equivalence -use-color --format html > /tmp/coverage.html
echo "open /tmp/coverage.html for HTML version of this report"
