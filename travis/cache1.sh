#!/bin/sh
#
# Script to fill Travis build cache
#

copy_tree()
{
  for x in $2/*; do
    if [ -d "$x" ]; then
      copy_tree $1 "$x" $3
    elif [ -f "$x" ] && [ "$x" == $3 ]; then
      mkdir -p `dirname $1/$x`
      cp $CP_OPTS "$x" "$1/$x" >>$OWBINDIR/cache1.log
    fi
  done
}

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
copy_tree . $OWROOT/buildx/w32api *.lib
#
cd $OWSRCDIR/clib/library
copy_tree . $OWROOT/buildx/clib/library *.lib
#
cd $OWSRCDIR/clib/doslfn/library
copy_tree . $OWROOT/buildx/clib/doslfn/library *.lib
#
cd $OWSRCDIR/clib/startup/library
copy_tree . $OWROOT/buildx/clib/startup/library *.obj
#
cd $OWSRCDIR/cpplib/library
copy_tree . $OWROOT/buildx/cpplib/library *.lib
#
#cd $OWSRCDIR/cpplib/runtime
#copy_tree . $OWROOT/buildx/cpplib/runtime *.obj
#
#cd $OWSRCDIR/cpplib/iostream
#copy_tree . $OWROOT/buildx/cpplib/iostream *.obj
#
cd $OWSRCDIR/mathlib/library
copy_tree . $OWROOT/buildx/mathlib/library *.lib
#
cd $OWSRCDIR/clib/rtdll
copy_tree . $OWROOT/buildx/clib/rtdll *.dll
copy_tree . $OWROOT/buildx/clib/rtdll *.lib
#
cd $OWSRCDIR/cpplib/rtdll
copy_tree . $OWROOT/buildx/cpplib/rtdll *.dll
copy_tree . $OWROOT/buildx/cpplib/rtdll *.lib
#
cd $OWSRCDIR/mathlib/rtdll
copy_tree . $OWROOT/buildx/mathlib/rtdll *.dll
copy_tree . $OWROOT/buildx/mathlib/rtdll *.lib
#
cd $TRAVIS_BUILD_DIR
