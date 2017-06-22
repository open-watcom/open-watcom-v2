#!/bin/sh
# *****************************************************************
# setowrel.sh - set OW release directory for output OW build
# *****************************************************************
#

mkdir -p ../travis-ci-ow-builds
cd ../travis-ci-ow-builds
export OWRELROOT=`pwd`
cd $TRAVIS_BUILD_DIR
echo "setowrel.sh - done"
