#! /bin/sh
# Check for command line option
if [ "$1" = "" ]; then
echo Usage: zipup-rel.sh [RELEASE]
echo
echo Where RELEASE is the public release number \(ie: 1.5.0\).
exit
fi

LABEL=open_watcom_$1
PREFIX=open_watcom_$1
P4OPT=-f
ARCHIVES=/archives
cd $ARCHIVES
rm -f $PREFIX-src.tar.bz2

#############################################################################
# Archive all Open Watcom source files
rm -rf $ARCHIVES/ow_src
p4 -cOPENWATCOM_SRC sync $P4OPT @$LABEL
cd $ARCHIVES/ow_src
tar -cv * | bzip2 -9 > ../$PREFIX-src.tar.bz2

cd ~/ow
