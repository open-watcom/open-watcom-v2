#!/bin/sh
# *****************************************************************
# setvars - Linux version
# *****************************************************************
# NOTE: Do not use this script directly, but copy it and
#       modify it as necessary for your own use!!

# Change this to point your Open Watcom source tree
export OWROOT=/home/ow/ow

# Set this entry to use native compiler tools
export OWUSENATIVETOOLS=0

# Subdirectory to be used for building OW build tools
export OWOBJDIR=binbuild

# Set this variable to 1 to get debug build
export OWDEBUGBUILD=0

# Set this variable to 1 to get default windowing support in clib
export OWDEFAULT_WINDOWING=0

# Change following entries to point your existing Open Watcom installation
if [ "$OWUSENATIVETOOLS" -ne "1" ]; then
    export WATCOM=/home/ow/ow19
    export INCLUDE=$WATCOM/lh
    export PATH=$WATCOM/binl:$PATH
endif

# Documentation related variables

# Set this variable to 0 to suppress documentation build
export OWDOCBUILD=0

# Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
# export OWGHOSTSCRIPTPATH=$PATH

# set appropriate variables to point to Windows help compilers which you have installed
# export OWWIN95HC=hcrtf
# export OWHHC=hhc

# Invoke the script for the common environment
. $OWROOT/cmnvars.sh

cd $OWROOT
