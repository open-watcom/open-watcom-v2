#!/bin/sh
# *****************************************************************
# gitget.sh - get clone of GitHub repository to return Travis CI
#               OW build/log files
# *****************************************************************
#
# configure Git client
#

gitget_proc()
{
    if [ "$OWTRAVIS_DEBUG" = "1" ]; then
        GITQUIET=-v
    else
        GITQUIET=--quiet
    fi
    
    echo_msg="gitget.sh - skipped"
    
    if [ "$TRAVIS_BRANCH" = "$OWBRANCH" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ]; then
            if [ "$OWTRAVISJOB" = "BOOTSTRAP" ] || [ "$OWTRAVISJOB" = "BUILD" ] || [ "$OWTRAVISJOB" = "DOCPDF" ]; then
                git config --global user.email "travis@travis-ci.org"
                git config --global user.name "Travis CI"
                git config --global push.default simple
                #
                # clone GitHub repository
                #
                git clone $GITQUIET --branch=$OWBRANCH https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
                echo_msg="gitget.sh - done"
            fi
        fi
    fi
    
    echo "$echo_msg"

    return 0
}

gitget_proc $*
