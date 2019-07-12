#!/bin/sh
#
# Script to fill Travis build cache2
#
set -x

copy_tree2_lib()
{
  for x in $2/*; do
    if [ -d "$x" ]; then
      copy_tree2_lib $1 "$x"
    elif [ -f "$x" ] && [ "$x" == "*.lib" ]; then
      f = $1/$x
      d = `dirname "$f"`
      if [ ! -d "$d" ]; then
        mkdir -p "$d" >>$OWBINDIR/cache2.log
      fi
      cp $CP_OPTS "$x" "$f" >>$OWBINDIR/cache2.log
    fi
  done
}

echo "save cache2" >$OWBINDIR/cache2.log
#
cd $OWSRCDIR/fpuemu
copy_tree2_lib . $OWROOT/buildx/fpuemu
#
cd $OWSRCDIR/wres
copy_tree2_lib . $OWROOT/buildx/wres
#
cd $OWSRCDIR/orl
copy_tree2_lib . $OWROOT/buildx/orl
#
cd $OWSRCDIR/owl
copy_tree2_lib . $OWROOT/buildx/owl
#
cd $OWSRCDIR/dwarf
copy_tree2_lib . $OWROOT/buildx/dwarf
#
cd $OWSRCDIR/cfloat
copy_tree2_lib . $OWROOT/buildx/cfloat
#
cd $OWSRCDIR/commonui
copy_tree2_lib . $OWROOT/buildx/commonui
#
cd $OWSRCDIR/wpi
copy_tree2_lib . $OWROOT/buildx/wpi
#
cd $TRAVIS_BUILD_DIR
