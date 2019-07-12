#!/bin/sh
#
# Script to fill Travis build cache2
#
set -x

copy_tree2()
{
  for x in $1/*; do
    if [ -f "$x" ]; then
      if [ "$x" == "$3" ]; then
        f = $2/$x
        d = `dirname "$f"`
        if [ ! -d "$d" ]; then
          mkdir -p "$d" >>$OWBINDIR/cache2.log
        fi
        cp $CP_OPTS "$x" "$f" >>$OWBINDIR/cache2.log
      fi
    elif [ -d "$x" ]; then
      copy_tree2 "$x" $2 $3
    fi
  done
}

copy_tree2_lib()
{
  cd $OWSRCDIR/$1
  copy_tree2 . $OWROOT/buildx/$1 *.lib
}

echo "save cache2" >$OWBINDIR/cache2.log
#
copy_tree2_lib fpuemu
#
copy_tree2_lib wres
#
copy_tree2_lib orl
#
copy_tree2_lib owl
#
copy_tree2_lib dwarf
#
copy_tree2_lib cfloat
#
copy_tree2_lib commonui
#
copy_tree2_lib wpi
#
cd $TRAVIS_BUILD_DIR
