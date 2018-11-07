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
            if [ "$1" = "wasm" ]; then
                cd $OWSRCDIR/wasmtest
                builder -q -i testclean
                builder -i test
                cat result.log
            elif [ "$1" = "c" ]; then
                cd $OWSRCDIR/ctest
                builder -q -i testclean
                builder -i test
                cat result.log
            elif [ "$1" = "f77" ]; then
                cd $OWSRCDIR/f77test
                builder -q -i testclean
                builder -i test
                cat result.log
            elif [ "$1" = "cpp" ]; then
                cd $OWSRCDIR/plustest
                builder -q -i testclean
                builder -i test
                cat result.log
            elif [ "$1" = "crtl" ]; then
                cd $OWSRCDIR/clibtest
                builder -q -i testclean
                builder -i test
                cat result.log
            else
            fi
        elif [ "$TRAVIS_EVENT_TYPE" = "pull_request" ] && [ "$TRAVIS_OS_NAME" = "linux" ]; then
            if [ "$1" = "wasm" ]; then
                cd $OWSRCDIR/wasmtest
                builder -q -i testclean
                builder -i test
                cat result.log
            elif [ "$1" = "c" ]; then
                cd $OWSRCDIR/ctest
                builder -q -i testclean
                builder -i test
                cat result.log
            elif [ "$1" = "f77" ]; then
                cd $OWSRCDIR/f77test
                builder -q -i testclean
                builder -i test
                cat result.log
            elif [ "$1" = "cpp" ]; then
                cd $OWSRCDIR/plustest
                builder -q -i testclean
                builder -i test
                cat result.log
            elif [ "$1" = "crtl" ]; then
                cd $OWSRCDIR/clibtest
                builder -q -i testclean
                builder -i test
                cat result.log
            else
            fi
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
