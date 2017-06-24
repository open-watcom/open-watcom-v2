#!/bin/sh
#
# Script to build the Open Watcom tools on Travis
# using the host platform's native C/C++ compiler.
#
# Expects 
#   - POSIX tools
#   - correct setup for all OW build environment variables
#

bootstrap_proc()
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
        if [ $RC -eq 0 ]; then
            cd $OWSRCDIR
            builder boot
            RC=$?
        fi
    fi
}

build_proc()
{
    if [ "$TRAVIS_BRANCH" = master ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ]; then
            if [ "$1" = "boot" ]; then
                bootstrap_proc
            else
                cd $OWSRCDIR
                builder $1
                RC=$?
            fi
        else
            if [ "$1" = "rel" ]; then
                bootstrap_proc
                if [ $RC -eq 0 ]; then
                    cd $OWSRCDIR
                    builder rel
                    RC=$?
                fi
            else
                RC=0
            fi
        fi
    elif [ "$COVERITY_SCAN_BRANCH" = 1 ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "cron" ]; then
            RC=0
        elif [ "$1" = "boot" ]; then
            bootstrap_proc
        elif [ "$1" = "rel" ]; then
            cd $OWSRCDIR
            builder $1
            RC=$?
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
