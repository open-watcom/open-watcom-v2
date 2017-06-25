#!/bin/sh
# *****************************************************************
# gitupds.sh - update git repository if build succeeds
# *****************************************************************
#
# If OW build succeeds then transfer OW build to GitHub repository
#

if [ "$TRAVIS_BRANCH" = "master" ]; then
    if [ "$OWTRAVISJOB" = "BUILD" ] || [ "$OWTRAVISJOB" = "DOCPDF" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ]; then
            if [ "$TRAVIS_OS_NAME" = "linux" ]; then
                #
                # commit updated files to GitHub repository
                #
                cd $OWRELROOT
                git add -f .
                if [ "$OWTRAVISJOB" = "DOCPDF" ]; then
                    git commit --quiet -m "Travis CI build $TRAVIS_JOB_NUMBER - Documentation"
                else
                    git commit --quiet -m "Travis CI build $TRAVIS_JOB_NUMBER - OW distribution"
                fi
                git push --quiet -f origin
                cd $TRAVIS_BUILD_DIR
                echo "gitupds.sh - done"
            else
                echo "gitupds.sh - skipped"
            fi
        else
            echo "gitupds.sh - skipped"
        fi
    else
        echo "gitupds.sh - skipped"
    fi
elif [ "$COVERITY_SCAN_BRANCH" = 1 ]; then
    echo "gitupds.sh - skipped"
else
    echo "gitupds.sh - skipped"
fi
