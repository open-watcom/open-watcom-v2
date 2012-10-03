#!/bin/sh
# *****************************************************************
# setvars - Linux version
# *****************************************************************
# NOTE: Do not use this script directly, but copy it and
#       modify it as necessary for your own use!!

# Change this to point your Open Watcom source tree
export OWROOT=/home/ow/ow

# Change following entries to point your existing Open Watcom installation
# or comment it out to use native compiler tools
export OWBOOTSTRAP=/home/ow/ow19
export WATCOM=$OWBOOTSTRAP
export INCLUDE=$WATCOM/lh
export PATH=$WATCOM/binl:$PATH

# Adjust to match the host platform
export BOOTSTRAP_OS=linux
export BOOTSTRAP_CPU=386

# Subdirectory to be used for building OW build tools
export OWOBJDIR=binbuild

# Set this variable to 1 to get debug build
export OWDEBUGBUILD=0

# Set this variable to 1 to get default windowing support in clib
export OWDEFAULT_WINDOWING=0

# Set this variable to 0 to suppress documentation build
export OWDOCBUILD=0

# Documentation related variables

# Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
# export OWGHOSTSCRIPTPATH=$PATH

# set appropriate variables to point to Windows help compilers which you have installed
# export OWWIN95HC=hcrtf
# export OWHHC=hhc

# Invoke the script for the common environment
. $OWROOT/cmnvars.sh
