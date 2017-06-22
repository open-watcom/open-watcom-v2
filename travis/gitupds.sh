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
        if [ "$TRAVIS_OS_NAME" = "linux" ]; then
            #
            # commit updated files to GitHub repository
            #
            cd $OWRELROOT
            git add -f .
            git commit --quiet -m "Travis CI build $TRAVIS_BUILD_NUMBER (success) - OW documentation"
            git push --quiet -f origin
            cd $TRAVIS_BUILD_DIR
            echo "gitupds.sh - done"
        else
            echo "gitupds.sh - skipped"
        fi
    else
        echo "gitupds.sh - skipped"
    fi
    ;;
  *)
    echo "gitupds.sh - skipped"
    ;;
esac
