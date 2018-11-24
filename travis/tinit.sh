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
        GITVERBOSE1=-v
        GITVERBOSE2=-v
    else
        GITVERBOSE1=--quiet
        GITVERBOSE2=
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
            git clone $GITVERBOSE1 --branch=master https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
            #
            # compress GitHub repository to hold only a few last builds
            #
            cd $OWTRAVIS_BUILD_DIR
            depth=`git rev-list HEAD --count`
            if [ $depth -gt 14 ]; then
                echo "tinit.sh - start compression"
                git checkout $GITVERBOSE2 --orphan temp1
                git add $GITVERBOSE2 -A
                git commit $GITVERBOSE1 -am "Initial commit"
                git branch $GITVERBOSE1 -D master
                git branch $GITVERBOSE1 -m master
                git push $GITVERBOSE1 -f origin master
                git branch $GITVERBOSE1 --set-upstream-to=origin/master master
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
