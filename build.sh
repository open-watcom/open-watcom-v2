#!/bin/sh
#
# Script to build the Open Watcom tools
# using the host platform's native C/C++ compiler or OW tools.
#
# Expects POSIX or OW tools.

cd $OWSRCDIR/wmake
if [ ! -d $OWOBJDIR ]; then mkdir $OWOBJDIR; fi
cd $OWOBJDIR
rm -f $OWBINDIR/wmake
if [ -n "$OWBOOTSTRAP" ]; then
    wmake -f ../wmake clean
    wmake -f ../wmake
else
    make -f ../posmake clean
    make -f ../posmake
fi
cd $OWSRCDIR/builder
if [ ! -d $OWOBJDIR ]; then mkdir $OWOBJDIR; fi
cd $OWOBJDIR
rm -f $OWBINDIR/builder
$OWBINDIR/wmake -f ../binmake clean
$OWBINDIR/wmake -f ../binmake bootstrap=1 builder.exe
cd $OWSRCDIR
builder boot
RC=$?
if [ $RC -eq 0 ]; then
    builder build
    RC=$?
fi
cd $OWROOT
return $RC
