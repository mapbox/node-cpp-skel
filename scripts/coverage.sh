#!/usr/bin/env bash

set -eu
set -o pipefail

# http://clang.llvm.org/docs/UsersManual.html#profiling-with-instrumentation
# https://www.bignerdranch.com/blog/weve-got-you-covered/

export CXX="$(pwd)/mason_packages/.link/bin/clang++"
export CXXFLAGS="-fprofile-instr-generate -fcoverage-mapping"
export LDFLAGS="-fprofile-instr-generate"
make debug
rm -f *profraw
rm -f *gcov
rm -f *profdata
LLVM_PROFILE_FILE="code-%p.profraw" npm test
CXX_MODULE=$(./node_modules/.bin/node-pre-gyp reveal module --silent)
$(pwd)/mason_packages/.link/bin/llvm-profdata merge -output=code.profdata code-*.profraw
$(pwd)/mason_packages/.link/bin/llvm-cov report ${CXX_MODULE} -instr-profile=code.profdata -use-color
$(pwd)/mason_packages/.link/bin/llvm-cov show ${CXX_MODULE} -instr-profile=code.profdata src/*.cpp -filename-equivalence -use-color
