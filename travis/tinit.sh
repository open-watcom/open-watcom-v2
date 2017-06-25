#!/bin/sh
# *****************************************************************
# tinit.sh - initialize
# *****************************************************************
#
# 1. compress GitHub repository if necessary
#

echo_msg="tinit.sh - skipped"

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    if [ "$TRAVIS_BRANCH" = "master" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ]; then
            #
            # configure Git client
            #
            git config --global user.email "travis@travis-ci.org"
            git config --global user.name "Travis CI"
            git config --global push.default simple
            #
            # clone GitHub repository
            #
            git clone $GITQUIET --branch=master https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
            #
            # compress GitHub repository to hold only a few last builds
            #
            cd $OWTRAVIS_BUILD_DIR
            depth=`git rev-list HEAD --count`
            if [ $depth -gt 12 ]; then
                echo "tinit.sh - start compression"
                git checkout --orphan temp1
                git add -A
                git commit -am "Initial commit"
                git branch -D master
                git branch -m master
                git push -f origin master
                git branch --set-upstream-to=origin/master master
                echo "tinit.sh - end compression"
            fi
            cd $TRAVIS_BUILD_DIR
            echo_msg="tinit.sh - done"
        fi
#    elif [ "$COVERITY_SCAN_BRANCH" = 1 ]; then
#    else
    fi
fi

echo "$echo_msg"
