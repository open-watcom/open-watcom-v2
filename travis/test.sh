#!/bin/sh
#
# Script to do regression tests of the Open Watcom tools on Travis
#
# Expects 
#   - copy of OW build for test
#   - correct setup for all OW environment variables
#

build_proc()
{
    if [ "$TRAVIS_BRANCH" = "$OWBRANCH" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ] && [ "$TRAVIS_OS_NAME" = "linux" ]; then
            cd $OWSRCDIR
            builder -i testclean
            builder -i test
        fi
    elif [ "$TRAVIS_BRANCH" = "$OWBRANCH_COVERITY" ]; then
        RC=0
    else
        RC=0
    fi
    cd $OWROOT
    return $RC
}

build_proc $*
