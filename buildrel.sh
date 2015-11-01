#!/bin/sh
#
# Script to build the Open Watcom tools
# using the host platform's native C/C++ compiler or OW tools.
#
# Expects POSIX or OW tools.

if [ -z "$OWROOT" ]; then
    source ./setvars.sh
fi

OWBUILDER_BOOTX_OUTPUT=$OWROOT/bootx.log

output_redirect()
{
    $1 $2 $3 $4 $5 $6 >>$OWBUILDER_BOOTX_OUTPUT 2>&1
}

cd $OWSRCDIR
builder rel
RC=$?
cd $OWROOT
exit $RC
