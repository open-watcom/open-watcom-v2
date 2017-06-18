#!/bin/sh
# *****************************************************************
# updgits.sh - update git repository if build succeeds
# *****************************************************************
#
# If OW build succeeds then transfer OW build to GitHub repository
#
if [ "$TRAVISJOBNAME" = "BUILDLINUX" ]; then
    cd ../travis-ci-ow-builds
    #
    # commit updated files to GitHub repository
    #
    if [ "$TRAVIS_PULL_REQUEST" = "false" ]; then
        if [ "$TRAVIS_OS_NAME" = "linux" ]; then
            git add -f .
            git commit --quiet -m "Travis CI build $TRAVIS_BUILD_NUMBER (success) - OW distribution"
            git push --quiet -f origin
        fi
    fi
    cd $TRAVIS_BUILD_DIR
fi
