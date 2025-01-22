#!/bin/sh
#
# Script to build the Open Watcom tools
# using the host platform's native C/C++ compiler or OW tools.
#
# Expects POSIX or OW tools.

if [ -z "$OWROOT" ]; then
    . ./setvars.sh
fi

if [ ! -d build/$OWOBJDIR ]; then mkdir build/$OWOBJDIR; fi

OWBUILDER_BOOTX_OUTPUT="$OWROOT/build/$OWOBJDIR/bootx.log"

output_redirect()
{
    $@ >>$OWBUILDER_BOOTX_OUTPUT 2>&1
}

rm -f "$OWBUILDER_BOOTX_OUTPUT"
cd bld/wmake
if [ ! -d $OWOBJDIR ]; then mkdir $OWOBJDIR; fi
cd $OWOBJDIR
rm -f ../../../build/$OWOBJDIR/wmake
if [ "$OWTOOLS" = "WATCOM" ]; then
    output_redirect wmake -m -f ../wmake clean
    output_redirect wmake -m -f ../wmake
else
    case `uname` in
        FreeBSD)
            output_redirect make -f ../posmake clean
            output_redirect make -f ../posmake TARGETDEF=-D__BSD__
            ;;
        Darwin)
            output_redirect make -f ../posmake clean
            output_redirect make -f ../posmake TARGETDEF=-D__OSX__
            ;;
        Haiku)
            output_redirect make -f ../posmake clean
            output_redirect make -f ../posmake TARGETDEF=-D__HAIKU__
            ;;
#        Linux)
        *)
            output_redirect make -f ../posmake clean
            output_redirect make -f ../posmake TARGETDEF=-D__LINUX__
            ;;
    esac
fi
RC=$?
if [ $RC -ne 0 ]; then
    echo "wmake bootstrap build error"
else
    cd "$OWROOT"
    cd bld/builder
    if [ ! -d $OWOBJDIR ]; then mkdir $OWOBJDIR; fi
    cd $OWOBJDIR
    rm -f ../../../build/$OWOBJDIR/builder
    output_redirect ../../../build/$OWOBJDIR/wmake -f ../preboot clean
    output_redirect ../../../build/$OWOBJDIR/wmake -f ../preboot
    cd "$OWROOT"
    if [ "$1" != "preboot" ]; then
        cd bld
        builder boot
        RC=$?
        if [ $RC -ne 0 ]; then
            echo "builder bootstrap build error"
        elif [ "$1" != "boot" ]; then
            if [ -z "$1" ]; then
                builder build
            else
                builder $1
            fi
            RC=$?
        fi
    fi
fi
cd "$OWROOT"
exit $RC
