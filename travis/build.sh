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
    if [ "$TRAVIS_BRANCH" = "$OWBRANCH" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "pull_request" ]; then
            if [ "$1" = "boot" ]; then
                bootutil_proc
                if [ $RC -eq 0 ]; then
                    cd $OWSRCDIR
                    builder boot
                    RC=$?
                fi
            elif [ "$1" = "build" ] || [ "$1" = "build1" ] || [ "$1" = "build2" ]; then
                if [ "$TRAVIS_OS_NAME" = "osx" ] && [ "$OWOSXBUILD" != "1" ]; then
                    return 0
                fi
                cd $OWSRCDIR
                builder $1
                RC=$?
            else
                RC=0
            fi
        else
            if [ "$1" = "boot" ]; then
                bootutil_proc
                if [ $RC -eq 0 ]; then
                    cd $OWSRCDIR
                    builder -q boot
                    RC=$?
                fi
            elif [ "$1" = "build" ] || [ "$1" = "build1" ] || [ "$1" = "build2" ]; then
                if [ "$TRAVIS_OS_NAME" = "osx" ] && [ "$OWOSXBUILD" != "1" ]; then
                    return 0
                fi
                cd $OWSRCDIR
                builder -q $1
                RC=$?
                if [ $RC -eq 0 ]; then
                    export OWRELROOT=$OWROOT/test
                    if [ "$1" = "boot" ]; then
                        builder -q cprel
                    elif [ "$1" = "build1" ]; then
                        builder -q cprel1
                    elif [ "$1" = "build2" ]; then
                        builder -q cprel2
                    fi
                fi
            elif [ "$1" = "docpdf" ]; then
                cd $OWSRCDIR
                builder docpdf
                RC=$?
            else
                cd $OWSRCDIR
                builder -q $1
                RC=$?
            fi
        fi
    elif [ "$TRAVIS_BRANCH" = "$OWBRANCH_COVERITY" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "cron" ]; then
            RC=0
        elif [ "$TRAVIS_EVENT_TYPE" = "push" ]; then
            if [ "$1" = "build" ]; then
                if [ "$TRAVIS_OS_NAME" = "osx" ] && [ "$OWOSXBUILD" != "1" ]; then
                    return 0
                fi
                travis/covscan.sh
            else
                RC=0
            fi
        else
            RC=0
        fi
    else
        RC=0
    fi
    cd $OWROOT
    return $RC
}

build_proc $*
