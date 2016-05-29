#!/bin/sh
#
# Script to build the Open Watcom tools
# using the host platform's native C/C++ compiler or OW tools.
#
# Expects POSIX or OW tools.

if [ -z "$OWROOT" ]; then
    . ./setvars.sh
fi

OWBUILDER_BOOTX_OUTPUT=$OWROOT/bootx.log

output_redirect()
{
    $1 $2 $3 $4 $5 $6 >>$OWBUILDER_BOOTX_OUTPUT 2>&1
}

die() {
    echo "Error: $1"
    exit 1
}

rm -f $OWBUILDER_BOOTX_OUTPUT || die "Failed to remove $OWBUILDER_BOOTX_OUTPUT"
cd $OWSRCDIR/wmake || die "Failed to enter $OWSRCDIR/wmake"
if [ ! -d $OWOBJDIR ]; then mkdir -p $OWOBJDIR || die "Failed to mkdir $OWOBJDIR"; fi
cd $OWOBJDIR || doe "Failed to enter $OWOWBDIR"
rm -f $OWBINDIR/wmake || die "Failed to remove $OWBINDIR/wmake"
if [ "$OWTOOLS" = "WATCOM" ]; then
    output_redirect wmake -f ../wmake clean || die 'Failed to wmake wmake clean'
    output_redirect wmake -f ../wmake || die 'Failed to wmake wmake'
else
    case `uname` in
        FreeBSD)
            output_redirect make -f ../posmake clean || die "Failed to make wmake clean"
            output_redirect make -f ../posmake TARGETDEF=-D__BSD__ || die "Failed to make wmake"
            ;;
        Haiku)
            output_redirect make -f ../posmake clean || die "Failed to make wmake clean"
            output_redirect make -f ../posmake TARGETDEF=-D__HAIKU__ || die "Failed to make wmake"
            ;;
#        Linux)
        *)
            output_redirect make -f ../posmake clean || die "Failed to make wmake clean"
            output_redirect make -f ../posmake TARGETDEF=-D__LINUX__ || die "Failed to make wmake"
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
    rm -f $OWBINDIR/builder
    output_redirect $OWBINDIR/wmake -f ../binmake clean
    output_redirect $OWBINDIR/wmake -f ../binmake bootstrap=1 builder.exe
    cd $OWSRCDIR
    builder boot
    RC=$?
    if [ $RC -ne 0 ]; then
        echo "builder bootstrap build error"
    else
        builder build
        RC=$?
        if [ $RC -ne 0 ]; then
            echo "builder bootstrap build error"
        else
            builder rel
            RC=$?
	fi
    fi
fi
cd $OWROOT
if [ $RC -eq 0 ]; then
    echo "Now you can run 'cd bld && builder rel' to produce the final binaries"
fi
exit $RC
