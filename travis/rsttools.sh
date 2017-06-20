#!/bin/sh
# *****************************************************************
# rsttools.sh - restore OW tools before OW build
# *****************************************************************
#
if [ "$TRAVIS_OS_NAME" = "osx" ]; then
    . travis/dropbox.sh download OSX/bin/ build/bin/
    . travis/dropbox.sh download OSX/watcom/ bld/watcom/binbuild/
else
    . travis/dropbox.sh download Linux/bin/ build/bin/
    . travis/dropbox.sh download Linux/watcom/ bld/watcom/binbuild/
fi
chmod +x build/bin/*
