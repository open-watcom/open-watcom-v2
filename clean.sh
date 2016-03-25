#!/bin/sh

find -name \*~ -delete
if [ -z "$OWROOT" ]; then
    source ./setvars.sh
fi
if [ ! -f $OWBINDIR/builder ]; then
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
rm -f $OWBINDIR/builder
rm -f $OWBINDIR/wmake
cd $OWROOT
