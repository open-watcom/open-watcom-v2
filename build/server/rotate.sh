#!/bin/sh
#
# Path configuration
# ==================
OWWWWPATH=/www
OWARCH7Z=7za
OWROOT=/home/ow/ow
OWRELROOT=$OWROOT/pass

if [ ! -d $OWRELROOT ]; then
    echo "Missing $OWRELROOT. Can't continue with rotation."
    exit -1
fi

if [ ! -d $OWWWWPATH/snaparch ]; then mkdir $OWWWWPATH/snaparch fi
if [ ! -d $OWWWWPATH/install ]; then mkdir $OWWWWPATH/install fi

# Build Archives
# ==============
rm -f $OWWWWPATH/snaparch/ss.7z
$OWARCH7Z a -t7z -r $OWWWWPATH/snaparch/ss.7z $OWRELROOT/*

# Move build to snapshot directory
# ================================
if [ -d $OWWWWPATH/snapshot ]; then
    if ! mv $OWWWWPATH/snapshot $OWWWWPATH/snapshot.bak; then
        echo "Existing snapshot backup failed. Can't continue with rotation."
        exit -1
    fi
fi
mv $OWRELROOT/ $OWWWWPATH/snapshot

# Move Archives
# =============
mv -f $OWWWWPATH/snaparch/ss.7z $OWWWWPATH/snaparch/ow-snapshot.7z

# Move installers
# =============
mv -f $OWROOT/distrib/ow/bin/* $OWWWWPATH/install/

# Final Cleanup
# =============
rm -rf $OWWWWPATH/snapshot.bak
