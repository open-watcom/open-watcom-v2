#!/bin/sh
# *****************************************************************
# tinit.sh - initialize
# *****************************************************************
#
# 1. compress GitHub repository if necessary
#

echo_msg="tinit.sh - skipped"

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    if [ "$TRAVIS_BRANCH" = "$OWBRANCH" ]; then
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
            git clone $GITQUIET --branch=$OWBRANCH https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
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
                git branch -D $OWBRANCH
                git branch -m $OWBRANCH
                git push -f origin $OWBRANCH
                git branch --set-upstream-to=origin/$OWBRANCH $OWBRANCH
                echo "tinit.sh - end compression"
            fi
            cd $TRAVIS_BUILD_DIR
            echo_msg="tinit.sh - done"
        fi
#    elif [ "$TRAVIS_BRANCH" = "$OWBRANCH_COVERITY" ]; then
#    else
    fi
fi

echo "$echo_msg"
