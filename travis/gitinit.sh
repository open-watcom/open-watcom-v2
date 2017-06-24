#!/bin/sh
# *****************************************************************
# gitinit.sh - initialize GitHub
# *****************************************************************
#
# 1. compress GitHub repository if necessary
#

if [ "$TRAVIS_PULL_REQUEST" = "false" ]; then
    #
    # configure Git client
    #
    git config --global user.email "travis@travis-ci.org"
    git config --global user.name "Travis CI"
    git config --global push.default simple
    #
    # clone GitHub repository
    #
    git clone --quiet --branch=master https://${GITHUB_TOKEN}@github.com/open-watcom/travis-ci-ow-builds.git ../travis-ci-ow-builds
    #
    # compress GitHub repository to hold only a few last builds
    #
    cd ../travis-ci-ow-builds
    depth=`git rev-list HEAD --count`
    if [ $depth -gt 12 ]; then
        echo "gitshrnk.sh - start compression"
        git checkout --orphan temp1
        git add -A
        git commit -am "Initial commit"
        git branch -D master
        git branch -m master
        git push -f origin master
        git branch --set-upstream-to=origin/master master
        echo "gitshrnk.sh - end compression"
    fi
    cd $TRAVIS_BUILD_DIR
    echo "gitshrnk.sh - done"
else
    echo "gitshrnk.sh - skipped"
fi
