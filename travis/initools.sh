#!/bin/sh
# *****************************************************************
# rsttools.sh - restore OW tools before OW build
# *****************************************************************
#

if [ "$TRAVIS_PULL_REQUEST" = "false" ]; then
    travis/cacheinf.sh
    echo "initools.sh - done"
else
    echo "initools.sh - skipped"
fi
