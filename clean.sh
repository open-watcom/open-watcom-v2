#!/bin/sh
if [ ! -f $OWBINDIR/builder ]; then
    echo Cannot find builder - did you run boot.sh?
else
    cd $OWSRCDIR
    builder -i clean
    builder -i bootclean
fi
# Nuke the builder and wmake bootstrap directories
if [ -d $OWSRCDIR/builder/$OWOBJDIR ]; then rm -rf $OWSRCDIR/builder/$OWOBJDIR; fi
if [ -d $OWSRCDIR/wmake/$OWOBJDIR ]; then rm -rf $OWSRCDIR/wmake/$OWOBJDIR; fi
# Finally delete the builder and wmake executables
rm -f $OWBINDIR/builder
rm -f $OWBINDIR/wmake
cd $OWROOT
