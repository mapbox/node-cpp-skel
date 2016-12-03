#!/usr/bin/env bash

set -eu
set -o pipefail

export NPM_FLAGS=${NPM_FLAGS:-}

# `is_pr_merge` is designed to detect if a gitsha represents a normal
# push commit (to any branch) or whether it represents travis attempting
# to merge between the origin and the upstream branch.
# For more details see: https://docs.travis-ci.com/user/pull-requests
function is_pr_merge() {
  # Get the commit message via git log
  # This should always be the exact text the developer provided
  export COMMIT_LOG=$(git log --format=%B --no-merges -n 1 | tr -d '\n')

  # Get the commit message via git show
  # If the gitsha represents a merge then this will
  # look something like "Merge e3b1981 into 615d2a3"
  # Otherwise it will be the same as the "git log" output
  export COMMIT_SHOW=$(git show -s --format=%B | tr -d '\n')

  if [[ "${COMMIT_LOG}" != "${COMMIT_SHOW}" ]]; then
     echo true
  fi
}

# `publish` is used to publish binaries to s3 via commit messages if:
# - the commit message includes [publish binary]
# - the commit message includes [republish binary]
# - the commit is not a pr_merge (checked with `is_pr_merge` function)
echo "dumping binary meta..."
./node_modules/.bin/node-pre-gyp reveal ${NPM_FLAGS}

echo "determining publishing status..."

if [[ $(is_pr_merge) ]]; then
    echo "Skipping publishing because this is a PR merge commit"
else
    echo "This is a push commit, continuing to package..."
    ./node_modules/.bin/node-pre-gyp package ${NPM_FLAGS}

    echo "gathering commit message ..."

    export COMMIT_MESSAGE=$(git log --format=%B --no-merges | head -n 1 | tr -d '\n')
    echo "Commit message: ${COMMIT_MESSAGE}"

    if [[ ${COMMIT_MESSAGE} =~ "[publish binary]" ]]; then
        echo "Publishing"
        ./node_modules/.bin/node-pre-gyp publish ${NPM_FLAGS}
    elif [[ ${COMMIT_MESSAGE} =~ "[republish binary]" ]]; then
        echo "Re-Publishing"
        ./node_modules/.bin/node-pre-gyp unpublish publish ${NPM_FLAGS}
    else
        echo "Skipping publishing"
    fi;
fi

# reset to make travis happy
set +eu
set +o pipefail