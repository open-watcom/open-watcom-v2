#!/bin/sh
# *****************************************************************
# rsttools.sh - restore OW tools before OW build
# *****************************************************************
#

if [ "$TRAVIS_PULL_REQUEST" = "false" ]; then
    ls -l ${TRAVIS_BUILD_DIR}/build/bin/*
    ls -l ${HOME}/mycache/*
    echo "initools.sh - done"
else
    echo "initools.sh - skipped"
fi
