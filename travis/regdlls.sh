#!/bin/sh
#
# Script to register Help compilers DLL's for build 
# the Open Watcom Documentation on Windows host by Travis
#
set -x

reg_proc()
{
    regsvr32 -u -s itcc.dll; true
    regsvr32 -s $OWTRAVISDIR/hhc/itcc.dll; true
    RC=$?

# sleep 3
    ping -n 3 127.0.0.1 >NUL
    return $RC
}

reg_proc $*
