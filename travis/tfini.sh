#!/bin/sh
# *****************************************************************
# tfini.sh - finalize all
# *****************************************************************

###################################################################
# 1. clear Travis cache
#

tfini_proc1()
{
    rm -f   build/$OWOBJDIR/*
#    rm -rf  bld/watcom/$OWOBJDIR/*
    rm -rf  bld/*
    rm -rf  test/*
    
    return 0
}

###################################################################
# 2. update GitHub repository coverity_scan branch by master branch
#

tfini_proc2()
{
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
            if [ "$OWTRAVIS_DEBUG" = "1" ]; then echo "** git checkout"; fi
            git checkout $OWBRANCH_COVERITY
            if [ "$OWTRAVIS_DEBUG" = "1" ]; then echo "** git merge"; fi
            git merge $GITQUIET $OWBRANCH
            if [ "$OWTRAVIS_DEBUG" = "1" ]; then echo "** git add"; fi
            git add $GITQUIET -A
            if [ "$OWTRAVIS_DEBUG" = "1" ]; then echo "** git commit"; fi
            git commit $GITQUIET -am "Travis CI update from $OWBRANCH branch"
            if [ "$OWTRAVIS_DEBUG" = "1" ]; then echo "** git push"; fi
            git push $GITQUIET -f origin $OWBRANCH_COVERITY
            #
            echo_msg="tfini.sh - done"
        fi
    fi
    
    echo "$echo_msg"
    
    return 0
}

tfini_proc1 $*
tfini_proc2 $*
