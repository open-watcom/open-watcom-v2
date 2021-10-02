#!/bin/sh
#
# Script to build the Open Watcom tools
# using the host platform's native C/C++ compiler or OW tools.
#
# Expects POSIX or OW tools.

if [ -z "$OWROOT" ]; then
    . ./setvars.sh
fi

if [ ! -d $OWBINDIR/$OWOBJDIR ]; then mkdir $OWBINDIR/$OWOBJDIR; fi

OWBUILDER_BOOTX_OUTPUT=$OWBINDIR/$OWOBJDIR/bootx.log

output_redirect()
{
    "$@" >>$OWBUILDER_BOOTX_OUTPUT 2>&1
}

rm -f $OWBUILDER_BOOTX_OUTPUT
cd $OWSRCDIR/wmake
if [ ! -d $OWOBJDIR ]; then mkdir $OWOBJDIR; fi
cd $OWOBJDIR
rm -f $OWBINDIR/$OWOBJDIR/wmake
if [ "$OWTOOLS" = "WATCOM" ]; then
    output_redirect wmake -f ../wmake clean
    output_redirect wmake -f ../wmake
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
    cd $OWSRCDIR/builder
    if [ ! -d $OWOBJDIR ]; then mkdir $OWOBJDIR; fi
    cd $OWOBJDIR
    rm -f $OWBINDIR/$OWOBJDIR/builder
    output_redirect $OWBINDIR/$OWOBJDIR/wmake -f ../binmake clean
    output_redirect $OWBINDIR/$OWOBJDIR/wmake -f ../binmake bootstrap=1
    if [ "$1" != "preboot" ]; then
        cd $OWSRCDIR
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
cd $OWROOT
exit $RC
