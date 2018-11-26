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
    # build new verison of wmake for host system
    #
    if [ ! -d $OWBINDIR ]; then mkdir $OWBINDIR; fi
    #
    cd $OWSRCDIR/wmake
    mkdir $OWOBJDIR
    cd $OWOBJDIR
    rm -f $OWBINDIR/wmake
    make -f ../posmake clean
    make -f ../posmake TARGETDEF=-D__LINUX__
    RC=$?
    if [ $RC -eq 0 ]; then
        #
        # build new verison of builder for host system
        #
        cd $OWSRCDIR/builder
        mkdir $OWOBJDIR
        cd $OWOBJDIR
        rm -f $OWBINDIR/builder
        $OWBINDIR/wmake -f ../binmake clean
        $OWBINDIR/wmake -f ../binmake bootstrap=1 builder.exe
        RC=$?
        if [ $RC -eq 0 ]; then
            cd $OWSRCDIR/watcom
            builder boot
            cd $OWSRCDIR/builder
            builder boot
            cd $OWSRCDIR/whpcvt
            builder boot
            cd $OWSRCDIR/bmp2eps
            builder boot
            RC=$?
            if [ $RC -eq 0 ]; then
                cd $OWSRCDIR
                builder docpdf
                RC=$?
            fi
        fi
    fi
    cd $OWROOT
    return $RC
}

build_proc $*
