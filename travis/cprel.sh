#!/bin/sh
#
# Script to build the Open Watcom tools on Travis
# using the host platform's native C/C++ compiler.
#
# Expects 
#   - POSIX tools
#   - correct setup for all OW build environment variables
#

cprel_proc()
{
    RC=0
    if [ "$TRAVIS_EVENT_TYPE" != "pull_request" ]; then
        cd $OWSRCDIR
        export OWRELROOT=$OWROOT/test
        case "$OWTRAVISJOB" in
            "BUILD")
                builder -q cprel
                RC=$?
                ;;
            "BUILD-1")
                builder -q cprel1
                RC=$?
                ;;
            "BUILD-2")
                builder -q cprel2
                RC=$?
                ;;
            "BUILD-3")
                builder -q cprel3
                RC=$?
                ;;
            *)
                cd $TRAVIS_BUILD_DIR
                ;;
        esac
        cd $TRAVIS_BUILD_DIR
    fi
    return $RC
}

cprel_proc $*
