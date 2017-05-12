#!/bin/sh
# *****************************************************************
# updgitf.sh - update git repository if build fails
# *****************************************************************
#
# after failure transfer log files back to GitHub repository
#
cd ../travis-ci-ow-builds
#
# copy build log files to git repository tree
#
if [ ! -d bld ]; then mkdir bld; fi
cp $OWROOT/bld/*.log bld/
if [ ! -d docs ]; then mkdir docs; fi
cp $OWROOT/docs/*.log docs/
#
# commit new log files to GitHub repository
#
git add -f .
git commit --quiet -m "Travis CI build $TRAVIS_BUILD_NUMBER (failure) - log files"
git push --quiet -f origin
cd $TRAVIS_BUILD_DIR
