#!/bin/sh
# *****************************************************************
# gitudocs.sh - update documentation branch to trigger build
# *****************************************************************

###################################################################
# 1. update GitHub repository documentation branch by master branch
#

gitudocs_proc1()
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
    cd ..
    rm -rf $TRAVIS_BUILD_DIR
    git clone $GITVERBOSE1 https://${GITHUB_TOKEN}@github.com/${TRAVIS_REPO_SLUG}.git $TRAVIS_BUILD_DIR
    cd $TRAVIS_BUILD_DIR
    #
    if [ "$OWTRAVIS_DEBUG" = "1" ]; then echo "** git checkout"; fi
    git checkout $OWBRANCH_DOCS
    if [ "$OWTRAVIS_DEBUG" = "1" ]; then echo "** git merge"; fi
    git merge $GITVERBOSE1 $OWBRANCH
    if [ "$OWTRAVIS_DEBUG" = "1" ]; then echo "** git add"; fi
    git add $GITVERBOSE2 -A
    if [ "$OWTRAVIS_DEBUG" = "1" ]; then echo "** git commit"; fi
    git commit $GITVERBOSE1 -am "Travis CI update from $OWBRANCH branch"
    if [ "$OWTRAVIS_DEBUG" = "1" ]; then echo "** git push"; fi
    git push $GITVERBOSE1 -f origin $OWBRANCH_DOCS
    #
    
    echo "gitudocs.sh - done"
    
    return 0
}

gitudocs_proc1 $*
