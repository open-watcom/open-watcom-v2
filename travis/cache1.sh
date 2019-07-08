#!/bin/sh
#
# Script to fill Travis build cache
#

echo "save cache1" >$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/hdr/dos >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/hdr/dos $OWROOT/buildx/hdr/dos/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/hdr/rdos >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/hdr/rdos $OWROOT/buildx/hdr/rdos/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/hdr/linux >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/hdr/linux $OWROOT/buildx/hdr/linux/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/os2api/os2286/h >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/os2api/os2286/h $OWROOT/buildx/os2api/os2286/h/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/os2api/os2386/h >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/os2api/os2386/h $OWROOT/buildx/os2api/os2386/h/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/w16api/wini86 >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/w16api/wini86 $OWROOT/buildx/w16api/wini86/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/w32api/nt >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/w32api/nt $OWROOT/buildx/w32api/nt/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/os2api/os2286/lib >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/os2api/os2286/lib/*.lib $OWROOT/buildx/os2api/os2286/lib/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/os2api/os2386/lib >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/os2api/os2386/lib/*.lib $OWROOT/buildx/os2api/os2386/lib/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/w32api/nt386 >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/w32api/nt386/*.lib $OWROOT/buildx/w32api/nt386/ >>$OWBINDIR/cache1.log
#mkdir -p $OWROOT/buildx/w32api/ntaxp >>$OWBINDIR/cache1.log
#cp $CP_OPTS $OWSRCDIR/w32api/ntaxp/*.lib $OWROOT/buildx/w32api/ntaxp/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/clib/library >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/clib/library $OWROOT/buildx/clib/library/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/clib/rtdll >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/clib/rtdll $OWROOT/buildx/clib/rtdll/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/cpplib/library >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/cpplib/library $OWROOT/buildx/cpplib/library/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/cpplib/rtdll >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/cpplib/rtdll $OWROOT/buildx/cpplib/rtdll/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/mathlib/library >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/mathlib/library $OWROOT/buildx/mathlib/library/ >>$OWBINDIR/cache1.log
mkdir -p $OWROOT/buildx/mathlib/rtdll >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/mathlib/rtdll $OWROOT/buildx/mathlib/rtdll/ >>$OWBINDIR/cache1.log
