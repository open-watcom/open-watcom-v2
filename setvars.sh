#!/bin/sh
# *****************************************************************
# setvars - Linux version
# *****************************************************************
# NOTE: Do not use this script directly, but copy it and
#       modify it as necessary for your own use!!

# Change this to point your Open Watcom source tree
#
# Note: '=' sign in path is not allowed (build will fail).
# Try to keep a OWROOT path short and simple like
# /tmp/ow There is dosemu used to build some parts
# of the software. dosemu can hang if OWROOT is long
# or contain long names of the directories.

export OWROOT=$(realpath `pwd`)

# Set this entry to identify your toolchain used by build process
# supported values are WATCOM GCC CLANG
export OWTOOLS=GCC

# Build control related variables
##################################

# Set this variable to 0/1 to suppress/enable documentation build
export OWDOCBUILD=0

# Set this variable to 1 to suppress tools GUI version build
# If it is used then only tools character mode version is build
# export OWGUINOBUILD=1

# Set this variable to list of OW projects to suppress their build
# Example export OWNOBUILD=ide browser dlgprs
# export OWNOBUILD=

# Documentation related variables
##################################

# Set this variable to 0 to suppress the GUI, IDE, and browser
export OWGUIBUILD=1

# Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
# export OWGHOSTSCRIPTPATH=$PATH

# Set appropriate variables to point to Windows help compilers which you 
# have installed
#
# OWWIN95HC specifies the Microsoft compiler used to build 32-bit ".hlp" 
#    files, common on Win 95/98/ME
# OWHHC specifies the Microsoft HTML Help Workshop compiler used to build 
#    Compressed HTML help ".chm" files, common on Win2k and later
# OWCHMCMD specifies the Free Pascal HTML Help compiler, an alternative 
#    compiler that can also build Compressed HTML help ".chm" files, 
#    available natively on non-Windows OSes
#export OWWIN95HC=hcrtf
#export OWHHC=hhc
#export OWCHMCMD=chmcmd

# build process requires WGML utility which is available only as DOS executable
# it is necessary to have some DOS emulator installed
# DOSBOX emulator is available on most platforms
# DOSEMU emulator is available on Linux (Intel platform)
# if DOSBOX emulator is used then OWDOSBOX variable must be set
# Uncoment and set OWDOSBOX variable bellow to point to DOSBOX emulator executable

# export OWDOSBOX=dosbox

##################################

# Invoke the script for the common environment
. $OWROOT/cmnvars.sh

cd $OWROOT
