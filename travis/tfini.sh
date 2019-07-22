#!/bin/sh
# *****************************************************************
# tfini.sh - finalize all
# *****************************************************************

###################################################################
# 1. clear Travis cache
#

tfini_proc1()
{
    rm -f   build/$OWOBJDIR/*
    rm -rf  bld/watcom/$OWOBJDIR/*
#    rm -rf  bld/*
    rm -rf  test/*
    rm -rf  buildx/*
    
    return 0
}

tfini_proc1 $*
