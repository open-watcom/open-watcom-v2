#set +v
# *****************************************************************
# setvars - Linux version
# *****************************************************************
# NOTE: Do not use this script directly, but copy it and
#       modify it as necessary for your own use!!

# Setup environment variables for CVS repository

# root of source tree
export OWROOT=`cd \`dirname -- $0\` && pwd`

# Change this to point to an existing Watcom compiler (if applicable)
export WATCOM=$OWROOT/rel2

# Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
export OWGHOSTSCRIPTPATH=/usr/bin

# Set this variable to 1 to get debug build
export DEBUG_BUILD=0

# Set this variable to 1 to get default windowing support in clib
export DEFAULT_WINDOWING=0

# Set this variable to 0 to suppress documentation build
export OWDOCBUILD=0      # Documentation building uses DOS programs.

# Documentation related variables - none likely to work in Linux
# set appropriate variables to point to Windows help compilers which you have installed
# export OWWIN95HC=hcrtf
# export OWHHC=hhc

# Subdirectory to be used for bootstrapping
export OWOBJDIR=bootstrp

# Subdirectory to be used for building prerequisite utilities
export OWPREOBJDIR=prebuild

# Invoke the script for the common environment
. $OWROOT/cmnvars.sh
