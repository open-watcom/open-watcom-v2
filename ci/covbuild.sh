#!/bin/sh
#
# Script to build the Open Watcom tools
# using the host platform's native C/C++ compiler or OW tools.
#
# Expects POSIX tools.

mkdir $OWBINDIR/$OWOBJDIR

mkdir $OWSRCDIR/wmake/$OWOBJDIR
cd $OWSRCDIR/wmake/$OWOBJDIR
if [ "$OWTOOLS" = "WATCOM" ]; then
    wmake -f ../wmake
else
    case `uname` in
        FreeBSD)
            make -f ../posmake TARGETDEF=-D__BSD__
            ;;
        Darwin)
            make -f ../posmake TARGETDEF=-D__OSX__
            ;;
        Haiku)
            make -f ../posmake TARGETDEF=-D__HAIKU__
            ;;
#        Linux)
        *)
            make -f ../posmake TARGETDEF=-D__LINUX__
            ;;
    esac
fi
RC=$?
if [ $RC -ne 0 ]; then
    echo "wmake bootstrap build error"
else
    export OWVERBOSE=1
    mkdir $OWSRCDIR/builder/$OWOBJDIR
    cd $OWSRCDIR/builder/$OWOBJDIR
    $OWBINDIR/$OWOBJDIR/wmake -f ../binmake bootstrap=1
    cd $OWSRCDIR
    builder boot
    RC=$?
    if [ $RC -ne 0 ]; then
        echo "builder bootstrap build error"
    else
        builder build cpu_x64 .and
        RC=$?
    fi
fi
cd $OWROOT
exit $RC
