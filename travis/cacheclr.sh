#!/bin/sh
#
# Script to clear Travis cache
#
#set -x

rm -rf build/$OWOBJDIR/*
rm -rf bld/watcom/$OWOBJDIR/*
rm -rf test/*
rm -rf buildx/*
true
