#!/bin/sh
# *****************************************************************
# tinit.sh - initialize all
# *****************************************************************

###################################################################
# 1. compress GitHub repository if necessary
#

tinit_proc1()
{
    if [ "$OWTRAVIS_DEBUG" = "1" ]; then
        GITQUIET=-v
    else
        GITQUIET=--quiet
    fi
    
    echo_msg="tinit.sh - skipped"
    
    if [ "$TRAVIS_BRANCH" = "$OWBRANCH" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ] && [ "$TRAVIS_OS_NAME" = "linux" ]; then
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
            if [ $depth -gt 14 ]; then
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
    fi
    
    echo "$echo_msg"
    
    return 0
}

tinit_proc1 $*
