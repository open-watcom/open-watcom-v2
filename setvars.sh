#!/bin/sh
# *****************************************************************
# setvars - Linux version
# *****************************************************************
# NOTE: Do not use this script directly, but copy it and
#       modify it as necessary for your own use!!

# Change this to point your Open Watcom source tree
#
# Note: '=' sign in path is not allowed (build will fail).

# /tmp/ow There is dosemu used to build some parts
# of the software. dosemu can hang if OWROOT is long
# or contain long names of the directories.

export OWROOT=$(realpath "`pwd`")

# Set this entry to identify your toolchain used by build process
# supported values are WATCOM GCC CLANG

export OWTOOLS=GCC

# Build control related variables
##################################

# Set this variable to 1 to enable documentation build

export OWDOCBUILD=0

# Set this variable to 1 to not use WGML utility
# It doesn't requires appropriate DOS emulator
# and suppress documentation build

# set OWNOWGML=1

# Set this variable to 1 to suppress tools GUI version build
# If it is used then only tools character mode version is build

# export OWGUINOBUILD=1

# Set this variable to list of OW projects to suppress their build
# Example export OWNOBUILD=ide browser dlgprs

# export OWNOBUILD=

# Set this variable to 1 to enable build all installers

export OWDISTRBUILD=0

# Documentation related variables
##################################

# Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
# export OWGHOSTSCRIPTPATH=:

# Set appropriate variables to point to Windows help compilers which you 
# have installed
#
# OWCHMCMD specifies the Free Pascal HTML Help compiler, an alternative 
#    compiler that can also build Compressed HTML help ".chm" files, 
#    available natively on non-Windows OSes
#export OWCHMCMD=chmcmd

# build process requires WGML utility which is available only as DOS executable
# it is necessary to have some DOS emulator installed
# DOSBOX emulator is available on most platforms
# if DOSBOX emulator is used then OWDOSBOX variable must be set
# Uncoment and set OWDOSBOX variable bellow to point to DOSBOX emulator executable

# export OWDOSBOX=dosbox

# Windows documentation build process requires Microsoft Help Compilers which can
# work only on Windows host therefore by default this build is disabled on non-Windows
# hosts
# On Linux is possible to use WINE (Windows emulation) to run these compilers
# This is experimental stuff to enable build Windows documentation on Linux host
# it requires fully functional WINE installation including Miscrosoft Help Compilers
# To enable this stuff uncomment following line

# export OWUSEWINE=1

##################################

# Subdirectory to be used for building OW build tools
# default is 'binbuild'
# export OWOBJDIR=binbuild

# Invoke the script for the common environment
. "$OWROOT/cmnvars.sh"

cd "$OWROOT"
