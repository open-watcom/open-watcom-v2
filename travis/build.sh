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
    RC=0
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
                cd $TRAVIS_BUILD_DIR
            fi
            ;;
        "BUILD")
            cd $OWSRCDIR
            if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                builder build
            else
                builder -q build
            fi
            RC=$?
            cd $TRAVIS_BUILD_DIR
            ;;
        "BUILD-1")
            cd $OWSRCDIR
            if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                builder build1
            else
                builder -q build1
            fi
            RC=$?
            cd $TRAVIS_BUILD_DIR
            ;;
        "BUILD-2")
            cd $OWSRCDIR
            if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                builder build2
            else
                builder -q build2
            fi
            RC=$?
            cd $TRAVIS_BUILD_DIR
            ;;
        "BUILD-3")
            cd $OWSRCDIR
            if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                builder build3
            else
                builder -q build3
            fi
            RC=$?
            cd $TRAVIS_BUILD_DIR
            ;;
        *)
            ;;
    esac
    return $RC
}

build_proc $*
