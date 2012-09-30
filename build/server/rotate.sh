#!/bin/sh
#
# Initialization
# ==============
wwwpath=/www
arch7z=7za
#
if [ ! -d $OWRELROOT ]; then
    echo "Missing $OWRELROOT. Can't continue with rotation."
    exit -1
fi

# Build Archives
# ==============
rm -f $wwwpath/snapshots/ss.zip
rm -f $wwwpath/snapshots/ss.7z
$arch7z a -tzip -r $wwwpath/snapshots/ss.zip $OWRELROOT/*
$arch7z a -t7z -r $wwwpath/snapshots/ss.7z $OWRELROOT/*

# Move pass1 build
# =================
if [ -d $wwwpath/snapshot ]; then
    if ! mv $wwwpath/snapshot $wwwpath/snapshot.bak; then
        echo "Existing snapshot backup failed. Can't continue with rotation."
        exit -1
    fi
fi
mv $OWRELROOT/ $wwwpath/snapshot

# Move Archives
# =============
mv -f $wwwpath/snapshots/ss.zip $wwwpath/snapshots/ow-snapshot.zip
mv -f $wwwpath/snapshots/ss.7z $wwwpath/snapshots/ow-snapshot.7z

# Move installers
# =============
mv -f $OWROOT/distrib/ow/open-watcom-* $wwwpath/installers/

# Final Cleanup
# =============
rm -rf $wwwpath/snapshot.bak
