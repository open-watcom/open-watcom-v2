#!/bin/sh
#
# Script to fill Travis build cache
#

echo "save cache1" >$OWBINDIR/cache1.log
mkdir -p buildx/hdr/dos >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/hdr/dos buildx/hdr/dos/ >>$OWBINDIR/cache1.log
mkdir -p buildx/hdr/rdos >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/hdr/rdos buildx/hdr/rdos/ >>$OWBINDIR/cache1.log
mkdir -p buildx/hdr/linux >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/hdr/linux buildx/hdr/linux/ >>$OWBINDIR/cache1.log
mkdir -p buildx/os2api/os2286/h >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/os2api/os2286/h buildx/os2api/os2286/h/ >>$OWBINDIR/cache1.log
mkdir -p buildx/os2api/os2386/h >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/os2api/os2386/h buildx/os2api/os2386/h/ >>$OWBINDIR/cache1.log
mkdir -p buildx/w16api/wini86 >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/w16api/wini86 buildx/w16api/wini86/ >>$OWBINDIR/cache1.log
mkdir -p buildx/w32api/nt >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/w32api/nt buildx/w32api/nt/ >>$OWBINDIR/cache1.log
mkdir -p buildx/os2api/os2286/lib >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/os2api/os2286/lib/*.lib buildx/os2api/os2286/lib/ >>$OWBINDIR/cache1.log
mkdir -p buildx/os2api/os2386/lib >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/os2api/os2386/lib/*.lib buildx/os2api/os2386/lib/ >>$OWBINDIR/cache1.log
mkdir -p buildx/w32api/nt386 >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/w32api/nt386/*.lib buildx/w32api/nt386/ >>$OWBINDIR/cache1.log
#mkdir -p buildx/w32api/ntaxp >>$OWBINDIR/cache1.log
#cp $CP_OPTS $OWSRCDIR/w32api/ntaxp/*.lib buildx/w32api/ntaxp/ >>$OWBINDIR/cache1.log
mkdir -p buildx/clib/library >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/clib/library buildx/clib/library/ >>$OWBINDIR/cache1.log
mkdir -p buildx/clib/rtdll >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/clib/rtdll buildx/clib/rtdll/ >>$OWBINDIR/cache1.log
mkdir -p buildx/cpplib/library >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/cpplib/library buildx/cpplib/library/ >>$OWBINDIR/cache1.log
mkdir -p buildx/cpplib/rtdll >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/cpplib/rtdll buildx/cpplib/rtdll/ >>$OWBINDIR/cache1.log
mkdir -p buildx/mathlib/library >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/mathlib/library buildx/mathlib/library/ >>$OWBINDIR/cache1.log
mkdir -p buildx/mathlib/rtdll >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/mathlib/rtdll buildx/mathlib/rtdll/ >>$OWBINDIR/cache1.log
