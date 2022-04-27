#!/bin/sh
#
# Script to build the Open Watcom tools on Azure Pipelines
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
    mkdir $OWBINDIR/$OWOBJDIR
    #
    RC=-1
    if [ "$OWTOOLS" = "WATCOM" ]; then
        mkdir $OWSRCDIR/wmake/$OWOBJDIR
        cd $OWSRCDIR/wmake/$OWOBJDIR
        wmake -f ../wmake
        RC=$?
    else
        case `uname` in
            Darwin)
                mkdir $OWSRCDIR/wmake/$OWOBJDIR
                cd $OWSRCDIR/wmake/$OWOBJDIR
                make -f ../posmake TARGETDEF=-D__OSX__
                RC=$?
                ;;
            *)
                mkdir $OWSRCDIR/wmake/$OWOBJDIR
                cd $OWSRCDIR/wmake/$OWOBJDIR
                make -f ../posmake TARGETDEF=-D__LINUX__
                RC=$?
                ;;
        esac
    fi
    if [ $RC -eq 0 ]; then
        #
        # build new verison of builder for host system
        #
        mkdir $OWSRCDIR/builder/$OWOBJDIR
        cd $OWSRCDIR/builder/$OWOBJDIR
        $OWBINDIR/$OWOBJDIR/wmake -f ../binmake bootstrap=1
        RC=$?
    fi
}

build_proc()
{
    if [ "$OWTOOLS" = "WATCOM" ]; then
        export PATH=$WATCOM_PATH:$PATH
    fi
    RC=0
    export OWDOSBOX=dosbox
    export OWGHOSTSCRIPTPATH=:

    . $OWROOT/cmnvars.sh

    export OWVERBOSE=1

    cd $OWSRCDIR
    case "$OWBUILD_STAGE" in
        "boot")
            bootutil_proc
            if [ $RC -eq 0 ]; then
                cd $OWSRCDIR
                builder boot
                RC=$?
            fi
            ;;
        "build")
            builder rel
            RC=$?
            ;;
        "tests")
            builder test $OWTESTTARGET
            RC=$?
            ;;
        "docs")
            builder docs $OWDOCTARGET
            RC=$?
            ;;
        "inst")
            builder install
            RC=$?
            ;;
        *)
            ;;
    esac
    cd $OWROOT
    return $RC
}

build_proc $*
