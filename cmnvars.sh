#!/bin/sh
# *****************************************************************
# cmnvars.sh - common environment variables
# *****************************************************************
# NOTE: All scripts to set the environment must call this script at
#       the end.

# Set the version numbers
export BLD_VER=20
export BLD_VER_STR=2.0

# Set up default path information variable
if [ -z "$OWDEFPATH" ]; then
    export OWDEFPATH=$PATH:
    export OWDEFINCLUDE=$INCLUDE
    export OWDEFWATCOM=$WATCOM
fi

# Subdirectory to be used for build binaries
export OWBINDIR=$OWROOT/build/bin

# Subdirectory containing OW sources
export OWSRCDIR=$OWROOT/bld

# Set environment variables
export PATH=$OWBINDIR:$OWROOT/build:$OWDEFPATH
export INCLUDE=$OWDEFINCLUDE
export WATCOM=$OWDEFWATCOM

echo Open Watcom build environment
