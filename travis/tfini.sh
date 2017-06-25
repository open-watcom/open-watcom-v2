#!/bin/sh
# *****************************************************************
# clrcache.sh - clear Travis cache
# *****************************************************************
#
# 1. update GitHub repository coverity_scan branch by master
#

rm -f   build/bin/*
rm -rf  bld/watcom/binbuild/*

echo_msg="tfini.sh - skipped"

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    if [ "$TRAVIS_BRANCH" != "master" ]; then
        if [ "$COVERITY_SCAN_BRANCH" = 1 ]; then
            if [ "$TRAVIS_EVENT_TYPE" = "cron" ]; then
                #
                # configure Git client
                #
                git config --global user.email "travis@travis-ci.org"
                git config --global user.name "Travis CI"
                git config --global push.default simple
                #
                cd ..
                rm -rf $TRAVIS_BUILD_DIR
                git clone $GITQUIET https://${GITHUB_TOKEN}@github.com/${TRAVIS_REPO_SLUG}.git $TRAVIS_BUILD_DIR
                cd $TRAVIS_BUILD_DIR
                #
                git checkout coverity_scan
                git merge master
                git add -A
                git commit $GITQUIET -am "Travis CI update from master branch"
                git push $GITQUIET -f origin coverity_scan
                #
                echo_msg="tfini.sh - done"
            fi
        fi
    fi
fi

echo "$echo_msg"
