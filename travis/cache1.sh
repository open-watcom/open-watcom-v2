#!/bin/sh
#
# Script to fill Travis build cache
#

cp $CP_OPTS $OWSRCDIR/hdr/dos buildx/hdr/dos/
cp $CP_OPTS $OWSRCDIR/hdr/rdos buildx/hdr/rdos/
cp $CP_OPTS $OWSRCDIR/hdr/linux buildx/hdr/linux/
cp $CP_OPTS $OWSRCDIR/os2api/os2286/h buildx/os2api/os2286/h/
cp $CP_OPTS $OWSRCDIR/os2api/os2386/h buildx/os2api/os2386/h/
cp $CP_OPTS $OWSRCDIR/w16api/wini86 buildx/w16api/wini86/
cp $CP_OPTS $OWSRCDIR/w32api/nt buildx/w32api/nt/
cp $CP_OPTS $OWSRCDIR/os2api/os2286/lib/*.lib buildx/os2api/os2286/lib/
cp $CP_OPTS $OWSRCDIR/os2api/os2386/lib/*.lib buildx/os2api/os2386/lib/
cp $CP_OPTS $OWSRCDIR/w32api/nt386/*.lib buildx/w32api/nt386/
cp $CP_OPTS $OWSRCDIR/w32api/ntaxp/*.lib buildx/w32api/ntaxp/
cp $CP_OPTS $OWSRCDIR/clib/library buildx/clib/library/
cp $CP_OPTS $OWSRCDIR/clib/rtdll buildx/clib/rtdll/
cp $CP_OPTS $OWSRCDIR/cpplib/library buildx/cpplib/library/
cp $CP_OPTS $OWSRCDIR/cpplib/rtdll buildx/cpplib/rtdll/
cp $CP_OPTS $OWSRCDIR/mathlib/library buildx/mathlib/library/
cp $CP_OPTS $OWSRCDIR/mathlib/rtdll buildx/mathlib/rtdll/
