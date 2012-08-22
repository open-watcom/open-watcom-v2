# *****************************************************************
# CMNVARS.SH - common environment variables
# *****************************************************************
# NOTE: All scripts to set the environment must call this script at
#       the end.

# Set the version numbers
export BLD_VER=20
export BLD_VER_STR=2.0

# Set up default path information variable
[ -n "$OWDEFPATH" ] || export OWDEFPATH=$PATH

# Stuff for the Open Watcom build environment
export BUILD_PLATFORM=linux386

# Subdirectory to be used for bootstrapping/prebuild binaries
export OWBINDIR=$OWROOT/bld/build/binl
export DWATCOM=$WATCOM
export INCLUDE=$WATCOM/lh
export EDPATH=$WATCOM/eddat
export PATH=$OWBINDIR:$OWROOT/build:$WATCOM/binl:$OWDEFPATH

echo Open Watcom compiler build environment

# OS specifics

export WD_PATH=$WATCOM/binl
export MAKE=make
