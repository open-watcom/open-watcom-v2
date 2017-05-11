#!/bin/sh
# *****************************************************************
# clrgittr.sh - compress git repository to hold maximum 10 commits
# *****************************************************************
cd ../travis-ci-ow-builds
depth = `git rev-list HEAD --count`
if [ $depth > -gt 10 ]; then
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
