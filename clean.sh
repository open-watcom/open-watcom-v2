#!/bin/sh
if [ -f setvars ]; then
    . ./setvars
else
    . ./setvars.sh
fi
if [ ! -f $OWBINDIR/wtouch ]; then
    cp -p `which touch` $OWBINDIR/wtouch
fi
cd bld
export BUILDMODE=bootstrap
builder clean
unset BUILDMODE

