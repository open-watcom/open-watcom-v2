#!/bin/sh
#
# Script to fill Travis build cache
#
#set -x

copy_tree1()
{
  cd $OWSRCDIR/$1
  dir1=$OWROOT/buildx/$1
  for x in `find . -name "$2" -type f`; do
      dirn=$dir1/`dirname "$x"`
      if [ ! -d "$dirn" ]; then
        mkdir -p "$dirn" >>$OWBINDIR/cache1.log
      fi
      cp $CP_OPTS "$x" "$dir1/$x" >>$OWBINDIR/cache1.log
  done
}

echo "save cache1" >$OWBINDIR/cache1.log
#
copy_tree1  hdr/dos                 "*"
copy_tree1  hdr/rdos                "*"
copy_tree1  hdr/linux               "*"
#
copy_tree1  os2api/os2286/h         "*"
copy_tree1  os2api/os2386/h         "*"
copy_tree1  w32api/nt               "*"
copy_tree1  w16api/wini86           "*.h"
#
copy_tree1  os2api/os2286/lib       "*.lib"
copy_tree1  os2api/os2386/lib       "*.lib"
copy_tree1  w32api                  "*.lib"
copy_tree1  w16api/wini86           "*.lib"
#
copy_tree1  clib/library            "*.lib"
copy_tree1  clib/doslfn/library     "*.lib"
copy_tree1  clib/startup/library    "*.obj"
copy_tree1  clib/rtdll              "*.dll"
copy_tree1  clib/rtdll              "*.lib"
#
copy_tree1  cpplib/library          "*.lib"
#copy_tree1  cpplib/runtime          "*.obj"
#copy_tree1  cpplib/iostream         "*.obj"
copy_tree1  cpplib/rtdll            "*.dll"
copy_tree1  cpplib/rtdll            "*.lib"
#
copy_tree1  mathlib/library         "*.lib"
copy_tree1  mathlib/rtdll           "*.dll"
copy_tree1  mathlib/rtdll           "*.lib"
#
cd $TRAVIS_BUILD_DIR
