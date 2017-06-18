#!/bin/sh
#
# Script to build the Open Watcom tools on Travis
# using the host platform's native C/C++ compiler or OW tools.
#
# Expects POSIX or OW tools.

if [ -z "$OWROOT" ]; then
    . ./setvars.sh
fi

OWBUILDER_BOOTX_OUTPUT=$OWROOT/bootx.log

output_redirect()
{
    "$@" >>$OWBUILDER_BOOTX_OUTPUT 2>&1
}

rm -f $OWBUILDER_BOOTX_OUTPUT

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
            output_redirect make -f ../posmake clean
            output_redirect make -f ../posmake TARGETDEF=-D__OSX__
            ;;
        *)
            output_redirect make -f ../posmake clean
            output_redirect make -f ../posmake TARGETDEF=-D__LINUX__
            ;;
    esac
    RC=$?
    if [ $RC -eq 0 ]; then
        #
        # build new verison of builder for host system
        #
        cd ../../builder
        mkdir $OWOBJDIR
        cd $OWOBJDIR
        rm -f $OWBINDIR/builder
        output_redirect $OWBINDIR/wmake -f ../binmake clean
        output_redirect $OWBINDIR/wmake -f ../binmake bootstrap=1 builder.exe
        builder boot
        RC=$?
    fi
    cd ../..
else
    builder $1
    RC=$?
fi
cd $OWROOT
exit $RC
