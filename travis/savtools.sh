#!/bin/sh
# *****************************************************************
# savtools.sh - save OW tools during bootstrap for use during build
# *****************************************************************
#
if [ "$TRAVIS_OS_NAME" = "osx" ]; then
    sh travis/dropbox.sh upload build/bin/ OSX/
else
    sh travis/dropbox.sh upload build/bin/ Linux/
fi
