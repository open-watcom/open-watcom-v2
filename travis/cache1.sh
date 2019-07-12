#!/bin/sh
#
# Script to fill Travis build cache
#
set -x

copy_tree1()
{
  for x in $1/*; do
    if [ -d "$x" ]; then
      copy_tree1 "$x" $2 $3
    elif [ -f "$x" ] && [ "$x" == "$3" ]; then
      f = $2/$x
      d = `dirname "$f"`
      if [ ! -d "$d" ]; then
        mkdir -p "$d" >>$OWBINDIR/cache1.log
      fi
      cp $CP_OPTS "$x" "$f" >>$OWBINDIR/cache1.log
    fi
  done
}

copy_tree1_obj()
{
  copy_tree1 . $1 "*.obj"
}

copy_tree1_lib()
{
  copy_tree1 . $1 "*.lib"
}

copy_tree1_dll()
{
  copy_tree1 . $1 "*.dll"
}

copy_tree1_h()
{
  copy_tree1 . $1 "*.h"
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
copy_tree1_h $OWROOT/buildx/w16api/wini86
copy_tree1_lib $OWROOT/buildx/w16api/wini86
#
cd $OWSRCDIR/w32api/nt
mkdir -p $OWROOT/buildx/w32api/nt >>$OWBINDIR/cache1.log
cp $CP_OPTS . $OWROOT/buildx/w32api/nt/ >>$OWBINDIR/cache1.log
#
cd $OWSRCDIR/os2api/os2286/lib
copy_tree1_lib $OWROOT/buildx/os2api/os2286/lib
#
cd $OWSRCDIR/os2api/os2386/lib
copy_tree1_lib $OWROOT/buildx/os2api/os2386/lib
#
cd $OWSRCDIR/w32api
copy_tree1_lib $OWROOT/buildx/w32api
#
cd $OWSRCDIR/clib/library
copy_tree1_lib $OWROOT/buildx/clib/library
#
cd $OWSRCDIR/clib/doslfn/library
copy_tree1_lib $OWROOT/buildx/clib/doslfn/library
#
cd $OWSRCDIR/clib/startup/library
copy_tree1_obj $OWROOT/buildx/clib/startup/library
#
cd $OWSRCDIR/cpplib/library
copy_tree1_lib $OWROOT/buildx/cpplib/library
#
#cd $OWSRCDIR/cpplib/runtime
#copy_tree1_obj $OWROOT/buildx/cpplib/runtime
#
#cd $OWSRCDIR/cpplib/iostream
#copy_tree1_obj $OWROOT/buildx/cpplib/iostream
#
cd $OWSRCDIR/mathlib/library
copy_tree1_lib $OWROOT/buildx/mathlib/library
#
cd $OWSRCDIR/clib/rtdll
copy_tree1_dll $OWROOT/buildx/clib/rtdll
copy_tree1_lib $OWROOT/buildx/clib/rtdll
#
cd $OWSRCDIR/cpplib/rtdll
copy_tree1_dll $OWROOT/buildx/cpplib/rtdll
copy_tree1_lib $OWROOT/buildx/cpplib/rtdll
#
cd $OWSRCDIR/mathlib/rtdll
copy_tree1_dll $OWROOT/buildx/mathlib/rtdll
copy_tree1_lib $OWROOT/buildx/mathlib/rtdll
#
cd $TRAVIS_BUILD_DIR
