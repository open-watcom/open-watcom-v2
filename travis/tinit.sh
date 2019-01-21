#!/bin/sh
# *****************************************************************
# tinit.sh - initialize all
# *****************************************************************

###################################################################
# 1. compress Travis GitHub repository if necessary
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
    
    #
    # configure Git client
    #
    git config --global user.email "travis@travis-ci.org"
    git config --global user.name "Travis CI"
    git config --global push.default simple
    #
    # clone GitHub repository
    #
    git clone $GITVERBOSE1 --branch=$OWBRANCH https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
    #
    # compress GitHub repository to hold only a few last builds
    #
    cd $OWTRAVIS_BUILD_DIR
    depth=`git rev-list HEAD --count`
    if [ $depth -gt 20 ]; then
        echo "tinit.sh - start compression"
        git checkout --orphan temp1
        git add $GITVERBOSE2 -A
        git commit $GITVERBOSE1 -am "Initial commit"
        git branch $GITVERBOSE1 -D $OWBRANCH
        git branch $GITVERBOSE1 -m $OWBRANCH
        git push $GITVERBOSE1 -f origin $OWBRANCH
        git branch $GITVERBOSE1 --set-upstream-to=origin/$OWBRANCH $OWBRANCH
        echo "tinit.sh - end compression"
    fi
    cd $TRAVIS_BUILD_DIR
    
    echo "tinit.sh - done"
    
    return 0
}

tinit_proc1 $*
