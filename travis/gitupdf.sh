#!/bin/sh
# *****************************************************************
# gitupdf.sh - update git repository if build fails
# *****************************************************************
#
# after failure transfer log files back to GitHub repository
#

if [ "$TRAVIS_BRANCH" = "master" ]; then
    if [ "$OWTRAVISJOB" = "BOOTSTRAP" ] || [ "$OWTRAVISJOB" = "BUILD" ] || [ "$OWTRAVISJOB" = "DOCPDF" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ]; then
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
            if [ "$OWTRAVIS_DEBUG" = "1" ]; then
                if [ "$TRAVIS_OS_NAME" = "osx" ]; then
                    git commit -m "Travis CI build $TRAVIS_JOB_NUMBER (failure) - log files (OSX)"
                else
                    git commit -m "Travis CI build $TRAVIS_JOB_NUMBER (failure) - log files (Linux)"
                fi
                git push -f origin
            else
                if [ "$TRAVIS_OS_NAME" = "osx" ]; then
                    git commit --quiet -m "Travis CI build $TRAVIS_JOB_NUMBER (failure) - log files (OSX)"
                else
                    git commit --quiet -m "Travis CI build $TRAVIS_JOB_NUMBER (failure) - log files (Linux)"
                fi
                git push --quiet -f origin
            fi
            cd $TRAVIS_BUILD_DIR
            echo "gitupdf.sh - done"
        else
            echo "gitupdf.sh - skipped"
        fi
    else
        echo "gitupdf.sh - skipped"
    fi
elif [ "$COVERITY_SCAN_BRANCH" = 1 ]; then
    echo "gitupdf.sh - skipped"
else
    echo "gitupdf.sh - skipped"
fi
