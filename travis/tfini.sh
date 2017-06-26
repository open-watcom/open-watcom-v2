#!/bin/sh
# *****************************************************************
# tfini.sh - finalize all
# *****************************************************************

###################################################################
# 1. clear Travis cache
#
rm -f   build/bin/*
rm -rf  bld/watcom/binbuild/*

###################################################################
# 2. update GitHub repository coverity_scan branch by master branch
#
if [ "$OWTRAVIS_DEBUG" = "1" ]; then
    GITQUIET=-v
else
    GITQUIET=--quiet
fi

echo_msg="tfini.sh - skipped"

if [ "$TRAVIS_BRANCH" = "$OWBRANCH_COVERITY" ]; then
    if [ "$TRAVIS_EVENT_TYPE" = "cron" ] && [ "$TRAVIS_OS_NAME" = "linux" ]; then
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
        git checkout $OWBRANCH_COVERITY
        git merge $OWBRANCH
        git add -A
        git commit $GITQUIET -am "Travis CI update from $OWBRANCH branch"
        git push $GITQUIET -f origin $OWBRANCH_COVERITY
        #
        echo_msg="tfini.sh - done"
    fi
fi

echo "$echo_msg"
