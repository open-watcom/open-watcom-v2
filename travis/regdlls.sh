#!/bin/sh
#
# Script to register Help compilers DLL's for build 
# the Open Watcom Documentation on Windows host by Travis
#
set -x

regsvr32 -u -s itcc.dll; true
regsvr32 -s $OWTRAVISBIN/hhc/itcc.dll; true
