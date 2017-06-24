#!/bin/sh
# *****************************************************************
# updgitf.sh - update git repository if build fails
# *****************************************************************
#
# after failure transfer log files back to GitHub repository
#

case "$OWTRAVISJOB" in
  BOOTSTRAP|BUILD|DOCPDF)
    if [ "$TRAVIS_PULL_REQUEST" = "false" ]; then
        cd $OWRELROOT
        #
        # remove all local changes to upload only error logs
        #
        git reset --hard
        git clean -fd
        #
        # copy build log files to git repository tree
        #
        if [ "$TRAVIS_OS_NAME" = "osx" ]; then
            if [ ! -d bld/osx ]; then mkdir -p logs/osx; fi
            cp $TRAVIS_BUILD_DIR/bld/*.log logs/osx/
        else
            if [ ! -d bld/linux ]; then mkdir -p logs/linux; fi
            cp $TRAVIS_BUILD_DIR/bld/*.log logs/linux/
        fi
        #
        # commit new log files to GitHub repository
        #
        git add -f .
        if [ "$TRAVIS_OS_NAME" = "osx" ]; then
            git commit --quiet -m "Travis CI build $TRAVIS_JOB_NUMBER (failure) - log files (OSX)"
        else
            git commit --quiet -m "Travis CI build $TRAVIS_JOB_NUMBER (failure) - log files (Linux)"
        fi
        git push --quiet -f origin
        cd $TRAVIS_BUILD_DIR
        echo "gitupdf.sh - done"
    else
        echo "gitupdf.sh - skipped"
    fi
    ;;
  *)
    echo "gitupdf.sh - skipped"
    ;;
esac
