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
    mkdir $OWROOT/build/$OWOBJDIR
    #
    RC=-1
    if [ "$OWTOOLS" = "WATCOM" ]; then
        mkdir $OWROOT/bld/wmake/$OWOBJDIR
        cd $OWROOT/bld/wmake/$OWOBJDIR
        wmake -m -f ../wmake
        RC=$?
    else
        case `uname` in
            Darwin)
                mkdir $OWROOT/bld/wmake/$OWOBJDIR
                cd $OWROOT/bld/wmake/$OWOBJDIR
                make -f ../posmake TARGETDEF=-D__OSX__
                RC=$?
                ;;
            *)
                mkdir $OWROOT/bld/wmake/$OWOBJDIR
                cd $OWROOT/bld/wmake/$OWOBJDIR
                make -f ../posmake TARGETDEF=-D__LINUX__
                RC=$?
                ;;
        esac
    fi
    if [ $RC -eq 0 ]; then
        #
        # build new verison of builder for host system
        #
        mkdir $OWROOT/bld/builder/$OWOBJDIR
        cd $OWROOT/bld/builder/$OWOBJDIR
        $OWROOT/build/$OWOBJDIR/wmake -f ../preboot
        RC=$?
    fi
}

build_proc()
{
    RC=0
    export OWGHOSTSCRIPTPATH=:

    . $OWROOT/cmnvars.sh

    export OWVERBOSE=1

    cd $OWROOT/bld
    case "$OWBUILD_STAGE" in
        "boot")
            bootutil_proc
            if [ $RC -eq 0 ]; then
                if [ "$OWDOCTARGET" = "" ]; then
                    cd $OWROOT/bld
                    builder boot
                    RC=$?
                else
                    cd $OWROOT/bld/watcom
                    builder boot
                    cd $OWROOT/bld/builder
                    builder boot
                    cd $OWROOT/bld/whpcvt
                    builder boot
                    cd $OWROOT/bld/bmp2eps
                    builder boot
                    RC=$?
                fi
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
            builder install $OWINSTTARGET
            RC=$?
            ;;
        *)
            ;;
    esac
    cd $OWROOT
    return $RC
}
if [ "$OWDEBUG" = "1" ]; then
    env | sort
fi
build_proc $*
