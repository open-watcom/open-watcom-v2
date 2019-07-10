#!/bin/sh
#
# Script to fill Travis build cache2
#

echo "save cache2" >$OWBINDIR/cache2.log
#
cd $OWSRCDIR/fpuemu/i86
mkdir -p $OWROOT/buildx/fpuemu/i86 >>$OWBINDIR/cache2.log
cp $CP_OPTS */*.lib $OWROOT/buildx/fpuemu/i86/ >>$OWBINDIR/cache2.log
#
cd $OWSRCDIR/fpuemu/386
mkdir -p $OWROOT/buildx/fpuemu/386 >>$OWBINDIR/cache2.log
cp $CP_OPTS */*.lib $OWROOT/buildx/fpuemu/386/ >>$OWBINDIR/cache2.log
#
cd $OWSRCDIR/wres
mkdir -p $OWROOT/buildx/wres >>$OWBINDIR/cache2.log
cp $CP_OPTS */*.lib $OWROOT/buildx/wres/ >>$OWBINDIR/cache2.log
#
cd $OWSRCDIR/orl
mkdir -p $OWROOT/buildx/orl >>$OWBINDIR/cache2.log
cp $CP_OPTS */*.lib $OWROOT/buildx/orl/ >>$OWBINDIR/cache2.log
#
cd $OWSRCDIR/owl
mkdir -p $OWROOT/buildx/owl >>$OWBINDIR/cache2.log
cp $CP_OPTS */*.lib $OWROOT/buildx/owl/ >>$OWBINDIR/cache2.log
#
cd $OWSRCDIR/dwarf/dw
mkdir -p $OWROOT/buildx/dwarf/dw >>$OWBINDIR/cache2.log
cp $CP_OPTS */*.lib $OWROOT/buildx/dwarf/dw/ >>$OWBINDIR/cache2.log
#
cd $OWSRCDIR/dwarf/dr
mkdir -p $OWROOT/buildx/dwarf/dr >>$OWBINDIR/cache2.log
cp $CP_OPTS */*.lib $OWROOT/buildx/dwarf/dr/ >>$OWBINDIR/cache2.log
#
cd $OWSRCDIR/cfloat
mkdir -p $OWROOT/buildx/cfloat >>$OWBINDIR/cache2.log
cp $CP_OPTS */*.lib $OWROOT/buildx/cfloat/ >>$OWBINDIR/cache2.log
#
cd $OWSRCDIR/commonui
mkdir -p $OWROOT/buildx/commonui >>$OWBINDIR/cache2.log
cp $CP_OPTS */*.lib $OWROOT/buildx/commonui/ >>$OWBINDIR/cache2.log
#
cd $OWSRCDIR/wpi
mkdir -p $OWROOT/buildx/wpi >>$OWBINDIR/cache2.log
cp $CP_OPTS */*.lib $OWROOT/buildx/wpi/ >>$OWBINDIR/cache2.log
#
cd $TRAVIS_BUILD_DIR
