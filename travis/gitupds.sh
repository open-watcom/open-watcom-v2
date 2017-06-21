#!/bin/sh
# *****************************************************************
# updgits.sh - update git repository if build succeeds
# *****************************************************************
#
# If OW build succeeds then transfer OW build to GitHub repository
#

case "$OWTRAVISJOB" in
  BUILDLINUX|PDFDOCLINUX)
    if [ "$TRAVIS_PULL_REQUEST" = "false" ]; then
        cd $OWRELROOT
        #
        # commit updated files to GitHub repository
        #
            if [ "$TRAVIS_OS_NAME" = "linux" ]; then
                git add -f .
                git commit --quiet -m "Travis CI build $TRAVIS_BUILD_NUMBER (success) - OW documentation"
                git push --quiet -f origin
            fi
        cd $TRAVIS_BUILD_DIR
    fi
    ;;
esac
