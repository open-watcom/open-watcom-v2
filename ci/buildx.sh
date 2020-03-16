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
    mkdir $OWBINDIR
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
        $OWBINDIR/wmake -f ../binmake bootstrap=1 builder.exe
        RC=$?
    fi
}

build_proc()
{
    if [ "$OWTOOLS" = "WATCOM" ]; then
        export PATH=$WATCOM/binl:$PATH
        export INCLUDE=$WATCOM/lh:$INCLUDE
        # export EDPATH=$WATCOM/eddat
    fi

    RC=0
    export OWDOSBOX=dosbox
    export OWGHOSTSCRIPTPATH=:

    . $OWROOT/cmnvars.sh

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
            export OWVERBOSE=1
            builder docs $OWDOCTARGET
            RC=$?
            ;;
        "inst")
            export OWVERBOSE=1
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
