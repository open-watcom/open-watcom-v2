#!/bin/sh

if [ -z "$OWROOT" ]; then
    . ./setvars.sh
fi
if [ ! -f $OWBINDIR/$OWOBJDIR/builder ]; then
    echo Cannot find builder - did you run build.sh?
else
    cd $OWSRCDIR
    builder -i clean
    builder -i bootclean
fi
# Delete the builder and wmake build directories
if [ -d $OWSRCDIR/builder/$OWOBJDIR ]; then rm -rf $OWSRCDIR/builder/$OWOBJDIR; fi
if [ -d $OWSRCDIR/wmake/$OWOBJDIR ]; then rm -rf $OWSRCDIR/wmake/$OWOBJDIR; fi
# Finally delete the builder and wmake executables
rm -f $OWBINDIR/$OWOBJDIR/builder
rm -f $OWBINDIR/$OWOBJDIR/wmake
cd $OWROOT
