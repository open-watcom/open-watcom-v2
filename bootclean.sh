#!/bin/sh
#
# Script to clean a 'bootstrap' build of Open Watcom tools.

if [ -f setvars ]; then
    . ./setvars
else
    . ./setvars.sh
fi
if [ ! -f $DEVDIR/build/binl/builder ]; then
    echo Cannot find builder - did you run boot.sh?
else
    cd bld
    builder -c boot.ctl clean
fi
