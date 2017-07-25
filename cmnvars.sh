#!/bin/sh
# *****************************************************************
# cmnvars.sh - common environment variables
# *****************************************************************
# NOTE: All scripts to set the environment must call this script at
#       the end.

# Set the version numbers
export OWBLDVER=20
export OWBLDVERSTR=2.0

# Subdirectory to be used for building OW build tools
if [ -z "$OWOBJDIR" ]; then export OWOBJDIR=binbuild; fi

# Subdirectory to be used for build binaries
export OWBINDIR=$OWROOT/build/$OWOBJDIR

# Subdirectory containing OW sources
export OWSRCDIR=$OWROOT/bld

# Subdirectory containing documentation sources
export OWDOCSDIR=$OWROOT/docs

# Set environment variables
# Set up default path information variable
if [ -z "$OWDEFPATH" ]; then
    export OWDEFPATH=$PATH:
    if [ -n "$INCLUDE" ]; then export OWDEFINCLUDE=$INCLUDE; fi
    if [ -n "$WATCOM" ]; then export OWDEFWATCOM=$WATCOM; fi
fi
export PATH=$OWBINDIR:$OWROOT/build:$OWDEFPATH
if [ -n "$OWDEFINCLUDE" ]; then export INCLUDE=$OWDEFINCLUDE; fi
if [ -n "$OWDEFWATCOM" ]; then export WATCOM=$OWDEFWATCOM; fi

# Set Watcom tool chain version to WATCOMVER variable
export WATCOMVER=0
if [ "$OWTOOLS" = "WATCOM" ]; then
    echo export WATCOMVER=__WATCOMC__>watcom.gc
    wcc386 -p watcom.gc >watcom.sh
    . ./watcom.sh
    rm watcom.*
fi

echo Open Watcom build environment
