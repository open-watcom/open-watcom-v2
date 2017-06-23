#!/bin/sh
# *****************************************************************
# savtools.sh - save OW tools during bootstrap for use during build
# *****************************************************************
#

if [ "$TRAVIS_PULL_REQUEST" = "false" ]; then
#    if [ "$TRAVIS_OS_NAME" = "osx" ]; then
#        travis/dropbox.sh upload build/bin/ OSX/bin/
#        travis/dropbox.sh upload bld/watcom/binbuild/clibext.lib OSX/watcom/
#        travis/dropbox.sh upload bld/watcom/binbuild/clibexts.lib OSX/watcom/
#    else
#        travis/dropbox.sh upload build/bin/ Linux/bin/
#        travis/dropbox.sh upload bld/watcom/binbuild/clibext.lib Linux/watcom/
#        travis/dropbox.sh upload bld/watcom/binbuild/clibexts.lib Linux/watcom/
#    fi
    ls -l ${TRAVIS_BUILD_DIR}/build/bin/*
#    cp build/bin/* ${HOME}/mycache/
    ls -l ${HOME}/mycache/*
    echo "savtools.sh - done"
else
    echo "savtools.sh - skipped"
fi
