#!/bin/sh
if [ -f setvars ]; then
    . ./setvars
else
    . ./setvars.sh
fi
if [ ! -f $DEVDIR/build/binl/wtouch ]; then
    cp -p `which touch` $DEVDIR/build/binl/wtouch
fi
cd bld
export BUILDMODE=bootstrap
builder clean
unset BUILDMODE

