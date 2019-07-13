#!/bin/sh
#
# Script to build the Open Watcom tools on Travis
# using the host platform's native C/C++ compiler.
#
# Expects 
#   - POSIX tools
#   - correct setup for all OW build environment variables
#


build_proc()
{
    RC=0
    cd $OWDOCSDIR
    if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
        builder rel
    else
        builder -q rel
    fi
    RC=$?
    cd $TRAVIS_BUILD_DIR
    return $RC
}

build_proc $*
