#!/bin/sh
#
# Script to fill Travis build cache2
#
set -x

copy_tree2()
{
  for x in $1/*; do
    if [ -d "$x" ]; then
      copy_tree2 "$x" $2 $3
    elif [ -f "$x" ] && [ "$x" == "$3" ]; then
      f = $2/$x
      d = `dirname "$f"`
      if [ ! -d "$d" ]; then
        mkdir -p "$d" >>$OWBINDIR/cache2.log
      fi
      cp $CP_OPTS "$x" "$f" >>$OWBINDIR/cache2.log
    fi
  done
}

copy_tree2_lib()
{
  copy_tree2 . $1 "*.lib"
}

echo "save cache2" >$OWBINDIR/cache2.log
#
cd $OWSRCDIR/fpuemu
copy_tree2 $OWROOT/buildx/fpuemu
#
cd $OWSRCDIR/wres
copy_tree2 $OWROOT/buildx/wres
#
cd $OWSRCDIR/orl
copy_tree2 $OWROOT/buildx/orl
#
cd $OWSRCDIR/owl
copy_tree2 $OWROOT/buildx/owl
#
cd $OWSRCDIR/dwarf
copy_tree2 $OWROOT/buildx/dwarf
#
cd $OWSRCDIR/cfloat
copy_tree2 $OWROOT/buildx/cfloat
#
cd $OWSRCDIR/commonui
copy_tree2 $OWROOT/buildx/commonui
#
cd $OWSRCDIR/wpi
copy_tree2 $OWROOT/buildx/wpi
#
cd $TRAVIS_BUILD_DIR
