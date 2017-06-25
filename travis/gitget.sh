#!/bin/sh
# *****************************************************************
# gitget.sh - get clone of GitHub repository to return Travis CI
#               OW build/log files
# *****************************************************************
#
# configure Git client
#

if [ "$TRAVIS_BRANCH" = "master" ]; then
    if [ "$OWTRAVISJOB" = "BOOTSTRAP" ] || [ "$OWTRAVISJOB" = "BUILD" ] || [ "$OWTRAVISJOB" = "DOCPDF" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ]; then
            git config --global user.email "travis@travis-ci.org"
            git config --global user.name "Travis CI"
            git config --global push.default simple
            #
            # clone GitHub repository
            #
            git clone --quiet --branch=master https://${GITHUB_TOKEN}@github.com/open-watcom/travis-ci-ow-builds.git ${OWRELROOT}
            echo "gitget.sh - done"
        else
            echo "gitget.sh - skipped"
        fi
    else
        echo "gitget.sh - skipped"
    fi
elif [ "$COVERITY_SCAN_BRANCH" = 1 ]; then
    echo "gitget.sh - skipped"
else
    echo "gitget.sh - skipped"
fi
