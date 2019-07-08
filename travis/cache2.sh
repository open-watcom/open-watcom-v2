#!/bin/sh
#
# Script to fill Travis build cache
#

echo "save cache2" >$OWBINDIR/cache2.log
mkdir -p $OWROOT/buildx/fpuemu/i86/stubi86 >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/fpuemu/i86/stubi86/*.lib $OWROOT/buildx/fpuemu/i86/stubi86/ >>$OWBINDIR/cache2.log
mkdir -p $OWROOT/buildx/fpuemu/i86/dosi86 >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/fpuemu/i86/dosi86/*.lib $OWROOT/buildx/fpuemu/i86/dosi86/ >>$OWBINDIR/cache2.log
mkdir -p $OWROOT/buildx/fpuemu/i86/os2i86 >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/fpuemu/i86/os2i86/*.lib $OWROOT/buildx/fpuemu/i86/os2i86/ >>$OWBINDIR/cache2.log
mkdir -p $OWROOT/buildx/fpuemu/i86/wini86 >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/fpuemu/i86/wini86/*.lib $OWROOT/buildx/fpuemu/i86/wini86/ >>$OWBINDIR/cache2.log
mkdir -p $OWROOT/buildx/fpuemu/386/stub386 >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/fpuemu/386/stub386/*.lib $OWROOT/buildx/fpuemu/386/stub386/ >>$OWBINDIR/cache2.log
mkdir -p $OWROOT/buildx/fpuemu/386/dos386 >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/fpuemu/386/dos386/*.lib $OWROOT/buildx/fpuemu/386/dos386/ >>$OWBINDIR/cache2.log
