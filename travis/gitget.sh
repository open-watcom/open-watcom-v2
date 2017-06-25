#!/bin/sh
# *****************************************************************
# gitget.sh - get clone of GitHub repository to return Travis CI
#               OW build/log files
# *****************************************************************
#
# configure Git client
#

echo_msg="gitget.sh - skipped"

if [ "$TRAVIS_BRANCH" = "master" ]; then
    if [ "$OWTRAVISJOB" = "BOOTSTRAP" ] || [ "$OWTRAVISJOB" = "BUILD" ] || [ "$OWTRAVISJOB" = "DOCPDF" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ]; then
            git config --global user.email "travis@travis-ci.org"
            git config --global user.name "Travis CI"
            git config --global push.default simple
            #
            # clone GitHub repository
            #
            if [ "$OWTRAVIS_DEBUG" = "1" ]; then pwd; fi
            git clone $GITQUIET --branch=master https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_SLUG}.git $OWTRAVIS_GITROOT
            echo_msg="gitget.sh - done"
        fi
    fi
#elif [ "$COVERITY_SCAN_BRANCH" = 1 ]; then
#else
fi

echo "$echo_msg"
