#!/bin/sh
# *****************************************************************
# savtools.sh - save OW tools during bootstrap for use during build
# *****************************************************************
#
if [ "$TRAVIS_OS_NAME" = "osx" ]; then
    sh travis/dropbox.sh upload build/bin/ OSX/bin/
    sh travis/dropbox.sh upload bld/watcom/binbuild/clibext.lib OSX/watcom/
    sh travis/dropbox.sh upload bld/watcom/binbuild/clibexts.lib OSX/watcom/
else
    sh travis/dropbox.sh upload build/bin/ Linux/bin/
    sh travis/dropbox.sh upload bld/watcom/binbuild/clibext.lib Linux/watcom/
    sh travis/dropbox.sh upload bld/watcom/binbuild/clibexts.lib Linux/watcom/
fi
