#!/bin/sh
#
# Path configuration
# ==================
WWWPATH=/www
ARCH7Z=7za
OWROOT=/home/ow/ow
OWRELROOT=/home/ow/ow/pass

if [ ! -d $OWRELROOT ]; then
    echo "Missing $OWRELROOT. Can't continue with rotation."
    exit -1
fi

# Build Archives
# ==============
rm -f $WWWPATH/snaparch/ss.zip
rm -f $WWWPATH/snaparch/ss.7z
$ARCH7Z a -tzip -r $WWWPATH/snaparch/ss.zip $OWRELROOT/*
$ARCH7Z a -t7z -r $WWWPATH/snaparch/ss.7z $OWRELROOT/*

# Move build to snapshot directory
# ================================
if [ -d $WWWPATH/snapshot ]; then
    if ! mv $WWWPATH/snapshot $WWWPATH/snapshot.bak; then
        echo "Existing snapshot backup failed. Can't continue with rotation."
        exit -1
    fi
fi
mv $OWRELROOT/ $WWWPATH/snapshot

# Move Archives
# =============
mv -f $WWWPATH/snaparch/ss.zip $WWWPATH/snaparch/ow-snapshot.zip
mv -f $WWWPATH/snaparch/ss.7z $WWWPATH/snaparch/ow-snapshot.7z

# Move installers
# =============
mv -f $OWROOT/distrib/ow/open-watcom-* $WWWPATH/install/

# Final Cleanup
# =============
rm -rf $WWWPATH/snapshot.bak
