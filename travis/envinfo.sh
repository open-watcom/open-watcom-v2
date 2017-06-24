#!/bin/sh
# *****************************************************************
# envinfo.sh - display environment variables
# *****************************************************************
#

if [ "$OWTRAVIS_DEBUG" = "1" ]; then
    env | sed -n -e '/^TRAVIS/p' -e '/^OW/p' -e '/^COVERITY/p' | sort
fi
