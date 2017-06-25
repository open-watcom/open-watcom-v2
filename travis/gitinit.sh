#!/bin/sh
# *****************************************************************
# gitinit.sh - initialize Git variables
# *****************************************************************
#

export OWTRAVIS_SLUG=open-watcom/travis-ci-ow-builds
export OWTRAVIS_GITROOT=${HOME}/build/$OWTRAVIS_SLUG
if [ "$OWTRAVIS_DEBUG" = "1" ]; then
    GITQUIET=
else
    GITQUIET=--quiet
fi
