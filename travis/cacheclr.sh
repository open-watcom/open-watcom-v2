#!/bin/sh
#
# Script to clear Travis cache
#
#set -x

rm -rf $OWBINDIR/*
rm -rf $OWSRCDIR/watcom/$OWOBJDIR/*
rm -rf $OWROOT/test/*
rm -rf $OWROOT/buildx/*
true
