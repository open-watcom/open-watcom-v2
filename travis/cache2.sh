#!/bin/sh
#
# Script to fill Travis build cache
#

echo "save cache2" >$OWBINDIR/cache2.log
mkdir -p buildx/fpuemu/i86/stubi86 >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/fpuemu/i86/stubi86/*.lib buildx/fpuemu/i86/stubi86/ >>$OWBINDIR/cache2.log
mkdir -p buildx/fpuemu/i86/dosi86 >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/fpuemu/i86/dosi86/*.lib buildx/fpuemu/i86/dosi86/ >>$OWBINDIR/cache2.log
mkdir -p buildx/fpuemu/i86/os2i86 >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/fpuemu/i86/os2i86/*.lib buildx/fpuemu/i86/os2i86/ >>$OWBINDIR/cache2.log
mkdir -p buildx/fpuemu/i86/wini86 >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/fpuemu/i86/wini86/*.lib buildx/fpuemu/i86/wini86/ >>$OWBINDIR/cache2.log
mkdir -p buildx/fpuemu/386/stub386 >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/fpuemu/386/stub386/*.lib buildx/fpuemu/386/stub386/ >>$OWBINDIR/cache2.log
mkdir -p buildx/fpuemu/386/dos386 >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/fpuemu/386/dos386/*.lib buildx/fpuemu/386/dos386/ >>$OWBINDIR/cache2.log
