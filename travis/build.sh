#!/bin/sh
#
# Script to build the Open Watcom tools on Travis
# using the host platform's native C/C++ compiler.
#
# Expects 
#   - POSIX tools
#   - correct setup for all OW build environment variables
#

bootutil_proc()
{
    #
    # build new verison of wmake for host system
    #
    if [ ! -d $OWBINDIR ]; then mkdir $OWBINDIR; fi
    #
    cd $OWSRCDIR/wmake
    mkdir $OWOBJDIR
    cd $OWOBJDIR
    rm -f $OWBINDIR/wmake
    if [ "$TRAVIS_OS_NAME" = "windows" ]; then
        nmake -f ../nmake clean
        nmake -f ../nmake
    else
        case `uname` in
            Darwin)
                make -f ../posmake clean
                make -f ../posmake TARGETDEF=-D__OSX__
                ;;
            *)
                make -f ../posmake clean
                make -f ../posmake TARGETDEF=-D__LINUX__
                ;;
        esac
    fi
    RC=$?
    if [ $RC -eq 0 ]; then
        #
        # build new verison of builder for host system
        #
        cd $OWSRCDIR/builder
        mkdir $OWOBJDIR
        cd $OWOBJDIR
        rm -f $OWBINDIR/builder
        $OWBINDIR/wmake -f ../binmake clean
        $OWBINDIR/wmake -f ../binmake bootstrap=1 builder.exe
        RC=$?
    fi
}

build_proc()
{
    case "$OWTRAVISJOB" in
        "BOOTSTRAP")
            bootutil_proc
            if [ $RC -eq 0 ]; then
                cd $OWSRCDIR
                if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                    builder boot
                else
                    builder -q boot
                fi
                RC=$?
            fi
            ;;
        "BUILD")
            cd $OWSRCDIR
            if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                builder build
                RC=$?
            else
                builder -q build
                RC=$?
                if [ $RC -eq 0 ]; then
                    export OWRELROOT=$OWROOT/test
                    builder -q cprel
                fi
            fi
            ;;
        "BUILD-1")
            cd $OWSRCDIR
            if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                builder build1
                RC=$?
            else
                builder -q build1
                RC=$?
                if [ $RC -eq 0 ]; then
                    export OWRELROOT=$OWROOT/test
                    builder -q cprel1
                fi
            fi
            ;;
        "BUILD-2")
            cd $OWSRCDIR
            if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                builder build2
                RC=$?
            else
                builder -q build2
                RC=$?
                if [ $RC -eq 0 ]; then
                    export OWRELROOT=$OWROOT/test
                    builder -q cprel2
                fi
            fi
            ;;
        "BUILD-3")
            cd $OWSRCDIR
            if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                builder build3
                RC=$?
            else
                builder -q build3
                RC=$?
                if [ $RC -eq 0 ]; then
                    export OWRELROOT=$OWROOT/test
                    builder -q cprel3
                fi
            fi
            ;;
        *)
            return 0
            ;;
    esac
    cd $TRAVIS_BUILD_DIR
    return $RC
}

build_proc $*
