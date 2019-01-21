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
    case "$OWTRAVISTEST" in
        "WASM")
            cd $OWSRCDIR/wasmtest
            builder -q -i testclean
            builder -i test
            RC=$?
            cat result.log
            ;;
        "C")
            cd $OWSRCDIR/ctest
            builder -q -i testclean
            builder -i test
            RC=$?
            cat result.log
            ;;
        "CXX")
            cd $OWSRCDIR/plustest
            builder -q -i testclean
            builder -i test
            RC=$?
            cat result.log
            ;;
        "F77")
            cd $OWSRCDIR/f77test
            builder -q -i testclean
            builder -i test
            RC=$?
            cat result.log
            ;;
        "CRTL")
            cd $OWSRCDIR/clibtest
            builder -q -i testclean
            builder -i test
# temporary set OK status until test will be ported to Linux
#            RC=$?
            RC=0
            cat result.log
            ;;
        *)
            RC=0
            ;;
    esac
    cd $TRAVIS_BUILD_DIR
    return $RC
}

build_proc $*
