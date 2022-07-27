#!/bin/sh

if [ -z "$OWROOT" ]; then
    . ./setvars.sh
fi
if [ ! -f "build/$OWOBJDIR/builder" ]; then
    echo Cannot find builder - did you run build.sh?
else
    cd bld
    builder -i clean
    builder -i bootclean
    cd "$OWROOT"
fi
# Delete the builder and wmake build directories
if [ -d "bld/builder/$OWOBJDIR" ]; then rm -rf "bld/builder/$OWOBJDIR"; fi
if [ -d "bld/wmake/$OWOBJDIR" ]; then rm -rf "bld/wmake/$OWOBJDIR"; fi
# Finally delete the builder and wmake executables
rm -f "build/$OWOBJDIR/builder"
rm -f "build/$OWOBJDIR/wmake"
