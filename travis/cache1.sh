#!/bin/sh
#
# Script to fill Travis build cache
#
set -x

copy_tree1()
{
  for x in $1/*; do
    if [ -f "$x" ]; then
      if [ "$x" == "$3" ]; then
        f = $2/$x
        d = `dirname "$f"`
        if [ ! -d "$d" ]; then
          mkdir -p "$d" >>$OWBINDIR/cache1.log
        fi
        cp $CP_OPTS "$x" "$f" >>$OWBINDIR/cache1.log
      fi
    elif [ -d "$x" ]; then
      copy_tree1 "$x" $2 $3
    fi
  done
}

copy_tree1_full()
{
  cd $OWSRCDIR/$1
  mkdir -p $OWROOT/buildx/$1 >>$OWBINDIR/cache1.log
  cp $CP_OPTS . $OWROOT/buildx/$1/ >>$OWBINDIR/cache1.log
}

copy_tree1_obj()
{
  cd $OWSRCDIR/$1
  copy_tree1 . $OWROOT/buildx/$1 *.obj
}

copy_tree1_lib()
{
  cd $OWSRCDIR/$1
  copy_tree1 . $OWROOT/buildx/$1 *.lib
}

copy_tree1_dll()
{
  cd $OWSRCDIR/$1
  copy_tree1 . $OWROOT/buildx/$1 *.dll
}

copy_tree1_h()
{
  cd $OWSRCDIR/$1
  copy_tree1 . $OWROOT/buildx/$1 *.h
}

echo "save cache1" >$OWBINDIR/cache1.log
#
copy_tree1_full hdr/dos
#
copy_tree1_full hdr/rdos
#
copy_tree1_full hdr/linux
#
copy_tree1_full os2api/os2286/h
#
copy_tree1_full os2api/os2386/h
#
copy_tree1_full w32api/nt
#
copy_tree1_h    w16api/wini86
copy_tree1_lib  w16api/wini86
#
copy_tree1_lib  os2api/os2286/lib
#
copy_tree1_lib  os2api/os2386/lib
#
copy_tree1_lib  w32api
#
copy_tree1_lib  clib/library
#
copy_tree1_lib  clib/doslfn/library
#
copy_tree1_obj  clib/startup/library
#
copy_tree1_lib  cpplib/library
#
#copy_tree1_obj cpplib/runtime
#
#copy_tree1_obj cpplib/iostream
#
copy_tree1_lib  mathlib/library
#
copy_tree1_dll  clib/rtdll
copy_tree1_lib  clib/rtdll
#
copy_tree1_dll  cpplib/rtdll
copy_tree1_lib  cpplib/rtdll
#
copy_tree1_dll  mathlib/rtdll
copy_tree1_lib  mathlib/rtdll
#
cd $TRAVIS_BUILD_DIR
