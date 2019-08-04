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
    cd $OWSRCDIR
    case "$TRAVIS_BUILD_STAGE_NAME" in
        "Bootstrap")
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
        "Build1")
            if [ "$TRAVIS_OS_NAME" = "linux" ]; then
                if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                    builder rel
                else
                    builder -q rel
                fi
            else
                if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                    builder rel1
                else
                    builder -q rel1
                fi
            fi
            RC=$?
            ;;
        "Build2")
            if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                builder rel2
            else
                builder -q rel2
            fi
            RC=$?
            ;;
        "Build3")
            if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                builder rel3
            else
                builder -q rel3
            fi
            RC=$?
            ;;
        "Documentation")
            export OWVERBOSE=1
            if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                builder docs
            else
                builder -q docs
            fi
            RC=$?
            ;;
        "Release")
            export OWVERBOSE=1
            builder missing
            if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
                builder install
            else
                builder -q install
            fi
            RC=$?
            ;;
        *)
            ;;
    esac
    cd $OWROOT
    return $RC
}

if [ "$TRAVIS_OS_NAME" = "windows" ]; then
    cmd.exe /c "travis\winbuild.cmd $*"
else
    build_proc $*
fi
