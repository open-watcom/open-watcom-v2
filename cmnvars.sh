#!/bin/sh
# *****************************************************************
# cmnvars.sh - common environment variables
# *****************************************************************
# NOTE: All scripts to set the environment must call this script at
#       the end.

# Set the version numbers
export OWBLDVER=20
export OWBLDVERSTR=2.0

# Set up default path information variable
if [ -z "$OWDEFPATH" ]; then
    export OWDEFPATH=$PATH:
    export OWDEFINCLUDE=$INCLUDE
    export OWDEFWATCOM=$WATCOM
fi

# Subdirectory to be used for building OW build tools
export OWOBJDIR=binbuild

# Subdirectory to be used for build binaries
export OWBINDIR=$OWROOT/build/bin

# Subdirectory containing OW sources
export OWSRCDIR=$OWROOT/bld

# Subdirectory containing documentation sources
export OWDOCSDIR=$OWROOT/docs

# Set environment variables
export PATH=$OWBINDIR:$OWROOT/build:$OWDEFPATH
export INCLUDE=$OWDEFINCLUDE
export WATCOM=$OWDEFWATCOM

# Set Watcom tool chain version to WATCOMVER variable
unset WATCOMVER
if [ "$OWTOOLS" = "WATCOM" ]; then
    echo export WATCOMVER=__WATCOMC__ >watcom.gc
    wcc386 -p watcom.gc >watcom.sh
    . ./watcom.sh
    rm watcom.*
fi

echo Open Watcom build environment
