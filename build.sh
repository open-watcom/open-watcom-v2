#!/bin/sh
#
# Script to build the Open Watcom tools
# using the host platform's native C/C++ compiler or OW tools.
#
# Expects POSIX or OW tools.

OWBUILDER_BOOTX_OUTPUT=$OWROOT/bootx.log

output_redirect()
{
    $1 $2 $3 $4 $5 $6 >>$OWBUILDER_BOOTX_OUTPUT 2>&1
}

rm -f $OWBUILDER_BOOTX_OUTPUT
cd $OWSRCDIR/wmake
if [ ! -d $OWOBJDIR ]; then mkdir $OWOBJDIR; fi
cd $OWOBJDIR
rm -f $OWBINDIR/wmake
if [ "$OWTOOLS" = "WATCOM" ]; then
    output_redirect wmake -f ../wmake clean
    output_redirect wmake -f ../wmake
else
    case `uname` in
        FreeBSD)
            output_redirect make -f ../posmake clean
            output_redirect make -f ../posmake TARGETDEF=-D__BSD__
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
cd $OWSRCDIR/builder
if [ ! -d $OWOBJDIR ]; then mkdir $OWOBJDIR; fi
cd $OWOBJDIR
rm -f $OWBINDIR/builder
output_redirect $OWBINDIR/wmake -f ../binmake clean
output_redirect $OWBINDIR/wmake -f ../binmake bootstrap=1 builder.exe
cd $OWSRCDIR
builder boot
RC=$?
if [ $RC -eq 0 ]; then
    builder build
    RC=$?
fi
cd $OWROOT
exit $RC
