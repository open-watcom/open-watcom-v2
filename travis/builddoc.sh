#!/bin/sh
#
# Script to build the Open Watcom tools on Travis
# using the host platform's native C/C++ compiler.
#
# Expects 
#   - POSIX tools
#   - correct setup for all OW build environment variables
#

build_proc()
{
    #
    # create bootstrap tools directory
    #
    mkdir $OWBINDIR
    #
    # build wmake for host system
    #
    cd $OWSRCDIR/wmake
    mkdir $OWOBJDIR
    cd $OWOBJDIR
    make -f ../posmake clean
    make -f ../posmake TARGETDEF=-D__LINUX__
    RC=$?
    #
    # build builder for host system
    #
    if [ $RC -eq 0 ]; then
        cd $OWSRCDIR/builder
        mkdir $OWOBJDIR
        cd $OWOBJDIR
        $OWBINDIR/wmake -f ../binmake clean
        $OWBINDIR/wmake -f ../binmake bootstrap=1 builder.exe
        RC=$?
    fi
    #
    # build all pre-requisity tools for documentation build
    #
    if [ $RC -eq 0 ]; then
        cd $OWSRCDIR/watcom
        builder boot
        RC=$?
    fi
    if [ $RC -eq 0 ]; then
        cd $OWSRCDIR/builder
        builder boot
        RC=$?
    fi
    if [ $RC -eq 0 ]; then
        cd $OWSRCDIR/whpcvt
        builder boot
        RC=$?
    fi
    if [ $RC -eq 0 ]; then
        cd $OWSRCDIR/bmp2eps
        builder boot
        RC=$?
    fi
    #
    # build documentation for Wiki
    #
    if [ $RC -eq 0 ]; then
        cd $OWSRCDIR
        builder -i doctrav -- -i
#        RC=$?
    fi
    cd $TRAVIS_BUILD_DIR
    return $RC
}

build_proc $*
