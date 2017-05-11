#!/bin/sh
# *****************************************************************
# clrgittr.sh - compress git repository to hold maximum 10 commits
# *****************************************************************
#
# configure Git client
#
git config --global user.email "travis@travis-ci.org"
git config --global user.name "Travis CI"
git config --global push.default simple
#
# clone GitHub repository
#
git clone --quiet --branch=master https://${GITHUB_TOKEN}@github.com/open-watcom/travis-ci-ow-builds.git ${TRAVIS_BUILD_DIR}/../travis-ci-ow-builds
#
# compress GitHub repository to hold only a few last builds
#
depth=0
cd ../travis-ci-ow-builds
depth=$(`git rev-list HEAD --count`)
if [ $depth -gt 10 ]
then
  git checkout --orphan temp1
  git add -A
  git commit -am "Initial commit"
  git branch -D master
  git branch -m master
  git push -f origin master
  git branch --set-upstream-to=origin/master master
  git pull
fi
cd $TRAVIS_BUILD_DIR
