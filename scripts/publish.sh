#!/usr/bin/env bash

set -eu
set -o pipefail

export COMMIT_MESSAGE=$(git log --format=%B --no-merges -n 1 | tr -d '\n')

# `is_pr_merge` is designed to detect if a gitsha represents a normal
# push commit (to any branch) or whether it represents travis attempting
# to merge between the origin and the upstream branch.
# For more details see: https://docs.travis-ci.com/user/pull-requests
function is_pr_merge() {
  # Get the commit message via git log
  # This should always be the exactly the text the developer provided
  local COMMIT_LOG=${COMMIT_MESSAGE}

  # Get the commit message via git show
  # If the gitsha represents a merge then this will
  # look something like "Merge e3b1981 into 615d2a3"
  # Otherwise it will be the same as the "git log" output
  export COMMIT_SHOW=$(git show -s --format=%B | tr -d '\n')

  if [[ "${COMMIT_LOG}" != "${COMMIT_SHOW}" ]]; then
     echo true
  fi
}

# Detect if this commit represents a tag. This is useful
# to detect if we are on a travis job that is running due to
# "git tags --push". In this case we don't want to publish even
# if [publish binary] is present since that should refer only to the
# previously job that ran for that commit and not the tag made
function is_tag_commit() {
  export COMMIT_MATCHES_KNOWN_TAG=$(git describe --exact-match $(git rev-parse HEAD) 2> /dev/null)
  if [[ ${COMMIT_MATCHES_KNOWN_TAG} ]]; then
    echo true
  fi
}

# `publish` is used to publish binaries to s3 via commit messages if:
# - the commit message includes [publish binary]
# - the commit message includes [republish binary]
# - the commit is not a pr_merge (checked with `is_pr_merge` function)
function publish() {
  echo "dumping binary meta..."
  ./node_modules/.bin/node-pre-gyp reveal --loglevel=error $@

  echo "determining publishing status..."

  if [[ $(is_pr_merge) ]]; then
      echo "Skipping publishing because this is a PR merge commit"
  elif [[  $(is_tag_commit) ]]; then
      echo "Skipping publishing because this is a tag"
  else
      echo "Commit message was: '${COMMIT_MESSAGE}'"

      if [[ ${COMMIT_MESSAGE} =~ "[publish binary]" ]]; then
          echo "Publishing"
          ./node_modules/.bin/node-pre-gyp package publish $@
      elif [[ ${COMMIT_MESSAGE} =~ "[republish binary]" ]]; then
          echo "Re-Publishing"
          ./node_modules/.bin/node-pre-gyp package unpublish publish $@
      else
          echo "Skipping publishing since we did not detect either [publish binary] or [republish binary] in commit message"
      fi
  fi
}

function usage() {
  >&2 echo "Usage"
  >&2 echo ""
  >&2 echo "$ ./scripts/publish.sh <args>"
  >&2 echo ""
  >&2 echo "All args are forwarded to node-pre-gyp like --debug"
  >&2 echo ""
  exit 1
}

# https://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash
for i in "$@"
do
case $i in
    -h | --help)
    usage
    shift
    ;;
    *)
    ;;
esac
done

publish $@
