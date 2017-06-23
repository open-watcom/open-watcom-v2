#!/bin/sh
# *****************************************************************
# setowenv.sh - set OW environment variables for OW build
# *****************************************************************
#

mkdir -p ../travis-ci-ow-builds
cd ../travis-ci-ow-builds
export OWRELROOT=`pwd`
cd $TRAVIS_BUILD_DIR
. ./cmnvars.sh
echo "setowenv.sh - done"
