#!/bin/sh
# *****************************************************************
# rsttools.sh - restore OW tools before OW build
# *****************************************************************
#
if [ "$TRAVIS_OS_NAME" = "osx" ]; then
    sh travis/dropbox.sh download OSX/ build/bin/
else
    sh travis/dropbox.sh download Linux/ build/bin/
fi
chmod +x build/bin/*
