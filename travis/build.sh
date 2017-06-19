#!/bin/sh
#
# Script to build the Open Watcom tools on Travis
# using the host platform's native C/C++ compiler or OW tools.
#
# Expects POSIX tools.

if [ -z "$OWROOT" ]; then
    . ./setvars.sh
fi

cd $OWSRCDIR
if [ "$1" = "boot" ]; then
    #
    # build new verison of wmake for host system
    #
    cd wmake
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
    cd ../..
    if [ $RC -eq 0 ]; then
        #
        # build new verison of builder for host system
        #
        cd builder
        mkdir $OWOBJDIR
        cd $OWOBJDIR
        rm -f $OWBINDIR/builder
        $OWBINDIR/wmake -f ../binmake clean
        $OWBINDIR/wmake -f ../binmake bootstrap=1 builder.exe
        cd ../..
        builder boot
        RC=$?
    fi
else
    builder $1
    RC=$?
fi
cd $OWROOT
exit $RC
