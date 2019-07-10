#!/bin/sh
#
# Script to fill Travis build cache2
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
#
mkdir -p $OWROOT/buildx/wres >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/wres/. $OWROOT/buildx/wres/ >>$OWBINDIR/cache2.log
mkdir -p $OWROOT/buildx/orl >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/orl/. $OWROOT/buildx/orl/ >>$OWBINDIR/cache2.log
mkdir -p $OWROOT/buildx/owl >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/owl/. $OWROOT/buildx/owl/ >>$OWBINDIR/cache2.log
mkdir -p $OWROOT/buildx/dwarf >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/dwarf/. $OWROOT/buildx/dwarf/ >>$OWBINDIR/cache2.log
mkdir -p $OWROOT/buildx/cfloat >>$OWBINDIR/cache2.log
cp $CP_OPTS $OWSRCDIR/cfloat/. $OWROOT/buildx/cfloat/ >>$OWBINDIR/cache2.log
