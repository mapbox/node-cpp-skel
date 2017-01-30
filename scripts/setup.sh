#!/usr/bin/env bash

set -eu
set -o pipefail

export MASON_RELEASE="${MASON_RELEASE:-0.5.0}"
export MASON_LLVM_RELEASE="${MASON_LLVM_RELEASE:-3.9.1}"

function run() {
    local config=${1}
    # unbreak bash shell due to rvm bug on osx: https://github.com/direnv/direnv/issues/210#issuecomment-203383459
    # this impacts any usage of scripts that are source'd (like this one)
    if [[ "${TRAVIS_OS_NAME:-}" == "osx" ]]; then
      echo 'shell_session_update() { :; }' > ~/.direnvrc
    fi

    #
    # MASON
    #

    function setup_mason() {
      local install_dir=${1}
      local mason_release=${2}
      if [[ ! -d ${install_dir} ]]; then
          mkdir -p ${install_dir}
          curl -sSfL https://github.com/mapbox/mason/archive/v${mason_release}.tar.gz | tar --gunzip --extract --strip-components=1 --directory=${install_dir}
      fi
    }

    setup_mason $(pwd)/.mason ${MASON_RELEASE}

    #
    # COMPILER
    #

    .mason/mason install clang++ ${MASON_LLVM_RELEASE}
    .mason/mason link clang++ ${MASON_LLVM_RELEASE}

    echo "export PATH=$(pwd)/.mason:$(pwd)/mason_packages/.link/bin:"'${PATH}' > ${config}
    echo "export CXX=$(pwd)/mason_packages/.link/bin/clang++" >> ${config}
    echo "export MASON_RELEASE=${MASON_RELEASE}" >> ${config}
    echo "export MASON_LLVM_RELEASE=${MASON_LLVM_RELEASE}" >> ${config}
    # https://github.com/google/sanitizers/wiki/AddressSanitizerAsDso
    RT_BASE=$(pwd)/mason_packages/.link/lib/clang/${MASON_LLVM_RELEASE}/lib/$(uname | tr A-Z a-z)/libclang_rt
    if [[ $(uname -s) == 'Darwin' ]]; then
        RT_PRELOAD=${RT_BASE}.asan_osx_dynamic.dylib
    else
        RT_PRELOAD=${RT_BASE}.asan-x86_64.so
    fi
    echo "export MASON_LLVM_RT_PRELOAD=${RT_PRELOAD}" >> ${config}
    SUPPRESSION_FILE="/tmp/leak_suppressions.txt"
    echo "leak:__strdup" > ${SUPPRESSION_FILE}
    echo "leak:v8::internal" >> ${SUPPRESSION_FILE}
    echo "leak:node::CreateEnvironment" >> ${SUPPRESSION_FILE}
    echo "leak:node::Init" >> ${SUPPRESSION_FILE}
    echo "export LSAN_OPTIONS=suppressions=${SUPPRESSION_FILE}" >> ${config}
    echo "export ASAN_OPTIONS=check_initialization_order=1:detect_stack_use_after_return=1" >> ${config}
    echo "export MASON_SANITIZE=address,integer,alignment,bounds,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,unsigned-integer-overflow,vla-bound" >> ${config}
    echo 'export MASON_SANITIZE_CXXFLAGS="-fsanitize=${MASON_SANITIZE} -fsanitize-address-use-after-scope -fno-omit-frame-pointer -fno-common"' >> ${config}
    echo 'export MASON_SANITIZE_LDFLAGS="-fsanitize=${MASON_SANITIZE}"' >> ${config}

    exit 0
}

function usage() {
  >&2 echo "Usage"
  >&2 echo ""
  >&2 echo "$ ./scripts/setup.sh --config local.env"
  >&2 echo "$ source local.env"
  >&2 echo ""
  exit 1
}

if [[ ! ${1:-} ]]; then
  usage
fi

# https://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash
for i in "$@"
do
case $i in
    --config)
    if [[ ! ${2:-} ]]; then
      usage
    fi
    shift
    run $@
    ;;
    -h | --help)
    usage
    shift
    ;;
    *)
    usage
    ;;
esac
done
