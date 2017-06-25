#!/bin/sh
# *****************************************************************
# gitupds.sh - update git repository if build succeeds
# *****************************************************************
#
# If OW build succeeds then transfer OW build to GitHub repository
#

echo_msg="gitupds.sh - skipped"

if [ "$TRAVIS_BRANCH" = "master" ]; then
    if [ "$OWTRAVISJOB" = "BUILD" ] || [ "$OWTRAVISJOB" = "DOCPDF" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ]; then
            if [ "$TRAVIS_OS_NAME" = "linux" ]; then
                #
                # commit updated files to GitHub repository
                #
                cd $OWTRAVIS_BUILD_DIR
                git add -f .
                if [ "$OWTRAVISJOB" = "DOCPDF" ]; then
                    git commit $GITQUIET -m "Travis CI build $TRAVIS_JOB_NUMBER - Documentation"
                else
                    git commit $GITQUIET -m "Travis CI build $TRAVIS_JOB_NUMBER - OW distribution"
                fi
                git push $GITQUIET -f origin
                cd $TRAVIS_BUILD_DIR
                echo_msg="gitupds.sh - done"
            fi
        fi
    fi
#elif [ "$COVERITY_SCAN_BRANCH" = 1 ]; then
#else
fi

echo "$echo_msg"
