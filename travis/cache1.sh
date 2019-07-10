#!/bin/sh
#
# Script to fill Travis build cache
#

echo "save cache1" >$OWBINDIR/cache1.log
#
cd $OWSRCDIR/hdr/dos
mkdir -p $OWROOT/buildx/hdr/dos >>$OWBINDIR/cache1.log
cp $CP_OPTS . $OWROOT/buildx/hdr/dos/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/hdr/rdos
mkdir -p $OWROOT/buildx/hdr/rdos >>$OWBINDIR/cache1.log
cp $CP_OPTS . $OWROOT/buildx/hdr/rdos/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/hdr/linux
mkdir -p $OWROOT/buildx/hdr/linux >>$OWBINDIR/cache1.log
cp $CP_OPTS $OWSRCDIR/hdr/linux/. $OWROOT/buildx/hdr/linux/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/os2api/os2286/h
mkdir -p $OWROOT/buildx/os2api/os2286/h >>$OWBINDIR/cache1.log
cp $CP_OPTS . $OWROOT/buildx/os2api/os2286/h/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/os2api/os2386/h
mkdir -p $OWROOT/buildx/os2api/os2386/h >>$OWBINDIR/cache1.log
cp $CP_OPTS . $OWROOT/buildx/os2api/os2386/h/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/w16api/wini86
mkdir -p $OWROOT/buildx/w16api/wini86 >>$OWBINDIR/cache1.log
cp $CP_OPTS *.h *.lib $OWROOT/buildx/w16api/wini86/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/w32api/nt
mkdir -p $OWROOT/buildx/w32api/nt >>$OWBINDIR/cache1.log
cp $CP_OPTS . $OWROOT/buildx/w32api/nt/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/os2api/os2286/lib
mkdir -p $OWROOT/buildx/os2api/os2286/lib >>$OWBINDIR/cache1.log
cp $CP_OPTS *.lib $OWROOT/buildx/os2api/os2286/lib/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/os2api/os2386/lib
mkdir -p $OWROOT/buildx/os2api/os2386/lib >>$OWBINDIR/cache1.log
cp $CP_OPTS *.lib $OWROOT/buildx/os2api/os2386/lib/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/w32api
mkdir -p $OWROOT/buildx/w32api >>$OWBINDIR/cache1.log
cp $CP_OPTS */*.lib */*/*.lib $OWROOT/buildx/w32api/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/clib/library
mkdir -p $OWROOT/buildx/clib/library >>$OWBINDIR/cache1.log
cp $CP_OPTS */*/*.lib $OWROOT/buildx/clib/library/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/clib/doslfn/library
mkdir -p $OWROOT/buildx/clib/doslfn/library >>$OWBINDIR/cache1.log
cp $CP_OPTS */*/*.lib $OWROOT/buildx/clib/doslfn/library/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/clib/startup/library
mkdir -p $OWROOT/buildx/clib/startup/library >>$OWBINDIR/cache1.log
cp $CP_OPTS */*/*.obj $OWROOT/buildx/clib/startup/library/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/cpplib/library
mkdir -p $OWROOT/buildx/cpplib/library >>$OWBINDIR/cache1.log
cp $CP_OPTS */*/*.lib $OWROOT/buildx/cpplib/library/ >>$OWBINDIR/cache1.log
#
#cd $OWSRCDIR/cpplib/runtime
#mkdir -p $OWROOT/buildx/cpplib/runtime >>$OWBINDIR/cache1.log
#cp $CP_OPTS */*.obj $OWROOT/buildx/cpplib/runtime/ >>$OWBINDIR/cache1.log
#
#cd $OWSRCDIR/cpplib/iostream
#mkdir -p $OWROOT/buildx/cpplib/iostream >>$OWBINDIR/cache1.log
#cp $CP_OPTS */*.obj $OWROOT/buildx/cpplib/iostream/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/mathlib/library
mkdir -p $OWROOT/buildx/mathlib/library >>$OWBINDIR/cache1.log
cp $CP_OPTS */*/*.lib $OWROOT/buildx/mathlib/library/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/clib/rtdll
mkdir -p $OWROOT/buildx/clib/rtdll >>$OWBINDIR/cache1.log
cp $CP_OPTS */*/*.lib */*/*.dll $OWROOT/buildx/clib/rtdll/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/cpplib/rtdll
mkdir -p $OWROOT/buildx/cpplib/rtdll >>$OWBINDIR/cache1.log
cp $CP_OPTS */*/*.lib */*/*.dll $OWROOT/buildx/cpplib/rtdll/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/mathlib/rtdll
mkdir -p $OWROOT/buildx/mathlib/rtdll >>$OWBINDIR/cache1.log
cp $CP_OPTS */*/*.lib */*/*.dll $OWROOT/buildx/mathlib/rtdll/ >>$OWBINDIR/cache1.log
#
cd $TRAVIS_BUILD_DIR
