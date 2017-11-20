#!/usr/bin/env bash

set -eu

# First create new repo on GitHub and copy the SSH repo url
# Then run "./scripts/liftoff.sh" from within your local node-cpp-skel root directory
# and it will create your new local project repo side by side with node-cpp-skel directory

echo "What is the name of your new project? "
read name
echo "What is the remote repo url for your new project? "
read url

mkdir ../$name
cp -R ../node-cpp-skel/. ../$name/
cd ../$name/
rm -rf .git
git init

git checkout -b node-cpp-skel-port
git add .
git commit -m "Port from node-cpp-skel"
git remote add origin $url
git push -u origin node-cpp-skel-port

# Perhaps useful for fresh start, also check out https://github.com/mapbox/node-cpp-skel#add-custom-code
# cp /dev/null CHANGELOG.md
# cp /dev/null README.md