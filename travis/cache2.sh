#!/bin/sh
#
# Script to fill Travis build cache2
#
#set -x

copy_tree2()
{
    cd $OWSRCDIR/$1
    dir1=$OWROOT/buildx/$1
    for x in `find . -name "$2" -type f`; do
        dirn=$dir1/`dirname "$x"`
        if [ ! -d "$dirn" ]; then
            mkdir -p "$dirn" >>$OWBINDIR/cache2.log
        fi
        cp $CP_OPTS "$x" "$dir1/$x" >>$OWBINDIR/cache2.log
    done
}

if [ "$1" = "load" ]; then
    echo "load cache" >>$OWBINDIR/cache2.log
    cp $CP_OPTS $OWROOT/buildx/. $OWSRCDIR/ >>$OWBINDIR/cache2.log; true
fi
if [ "$1" = "save" ]; then
    echo "save cache2" >$OWBINDIR/cache2.log
    #
    copy_tree2  fpuemu          "*.lib"
    copy_tree2  wres            "*.lib"
    copy_tree2  orl             "*.lib"
    copy_tree2  owl             "*.lib"
    copy_tree2  dwarf           "*.lib"
    copy_tree2  cfloat          "*.lib"
    copy_tree2  win386/conv     "*.lib"
    copy_tree2  win386/ext      "*.ext"
    copy_tree2  wstuba/dos386   "*.exe"
    #
    cd $TRAVIS_BUILD_DIR
fi
