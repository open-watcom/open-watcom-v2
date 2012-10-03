#!/bin/sh
#
# Path configuration
# ==================
wwwpath=/www
arch7z=7za
export OWROOT=/home/ow/ow
export OWRELROOT=/home/ow/ow/pass1

# Initialization
# ==============

. $OWROOT/cmnvars.sh

if [ ! -d $OWRELROOT ]; then
    echo "Missing $OWRELROOT. Can't continue with rotation."
    exit -1
fi

# Build Archives
# ==============
rm -f $wwwpath/snaparch/ss.zip
rm -f $wwwpath/snaparch/ss.7z
$arch7z a -tzip -r $wwwpath/snaparch/ss.zip $OWRELROOT/*
$arch7z a -t7z -r $wwwpath/snaparch/ss.7z $OWRELROOT/*

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
mv -f $wwwpath/snaparch/ss.zip $wwwpath/snaparch/ow-snapshot.zip
mv -f $wwwpath/snaparch/ss.7z $wwwpath/snaparch/ow-snapshot.7z

# Move installers
# =============
mv -f $OWROOT/distrib/ow/open-watcom-* $wwwpath/install/

# Final Cleanup
# =============
rm -rf $wwwpath/snapshot.bak
