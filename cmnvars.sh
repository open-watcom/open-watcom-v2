#!/bin/sh
# *****************************************************************
# cmnvars.sh - common environment variables
# *****************************************************************
# NOTE: All scripts to set the environment must call this script at
#       the end.

# Set the version numbers
export OWBLDVER=20
export OWBLDVERSTR=2.0
export OWBLDVERTOOL=1300

# Subdirectory to be used for building OW build tools
if [ -z "$OWOBJDIR" ]; then export OWOBJDIR=binbuild; fi

# Set environment variables
# Set up default path information variable
if [ -z "$OWDEFPATH" ]; then
    export OWDEFPATH="$PATH":
    if [ -n "$INCLUDE" ]; then export OWDEFINCLUDE=$INCLUDE; fi
    if [ -n "$WATCOM" ]; then export OWDEFWATCOM=$WATCOM; fi
fi
export PATH="$OWROOT/build/$OWOBJDIR:$OWROOT/build:$OWDEFPATH"
if [ -n "$OWDEFINCLUDE" ]; then export INCLUDE=$OWDEFINCLUDE; fi
if [ -n "$OWDEFWATCOM" ]; then export WATCOM=$OWDEFWATCOM; fi

# Set the toolchain version to OWTOOLSVER variable
export OWTOOLSVER=0
if [ "$OWTOOLS" = "WATCOM" ]; then
    echo export OWTOOLSVER=__WATCOMC__>getversi.gc
    wcc386 -p getversi.gc >getversi.sh
elif [ "$OWTOOLS" = "CLANG" ]; then
    export CC=clang
    export CXX=clang++
    echo export OWTOOLSVER=__clang_major__>getversi.gc
    clang -x c -E getversi.gc -o getversi.sh
elif [ "$OWTOOLS" = "GCC" ]; then
    export CC=gcc
    export CXX=g++
    echo export OWTOOLSVER=__GNUC__>getversi.gc
    gcc -x c -E getversi.gc -o getversi.sh
elif [ "$OWTOOLS" = "VISUALC" ]; then
    CC=cl
    CXX=cl
    echo export OWTOOLSVER=_MSC_VER>getversi.gc
    cl -nologo -EP getversi.gc>getversi.bat
fi
if [ -f ./getversi.sh ]; then
    . ./getversi.sh
    rm getversi.*
fi

echo "Open Watcom build environment (${OWTOOLS} version=${OWTOOLSVER})"
