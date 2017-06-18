#!/bin/sh
# *****************************************************************
# updgitf.sh - update git repository if build fails
# *****************************************************************
#
# after failure transfer log files back to GitHub repository
#
cd ../travis-ci-ow-builds
git pull
#
# copy build log files to git repository tree
#
if [ ! -d bld ]; then mkdir bld; fi
cp $OWROOT/bld/*.log bld/
#if [ ! -d docs ]; then mkdir docs; fi
#cp $OWROOT/docs/*.log docs/
#
# commit new log files to GitHub repository
#
git add -f .
if [ "$TRAVIS_OS_NAME" = "osx" ]
then
  git commit --quiet -m "Travis CI build $TRAVIS_BUILD_NUMBER (failure) - log files (OSX)"
else
  git commit --quiet -m "Travis CI build $TRAVIS_BUILD_NUMBER (failure) - log files (Linux)"
fi
git push --quiet -f origin
cd $TRAVIS_BUILD_DIR
