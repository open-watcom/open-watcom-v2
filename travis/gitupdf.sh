#!/bin/sh
# *****************************************************************
# gitupdf.sh - update git repository if build fails
# *****************************************************************
#
# after failure transfer log files back to GitHub repository
#

echo_msg="gitupdf.sh - skipped"

if [ "$TRAVIS_BRANCH" = "master" ]; then
    if [ "$OWTRAVISJOB" = "BOOTSTRAP" ] || [ "$OWTRAVISJOB" = "BUILD" ] || [ "$OWTRAVISJOB" = "DOCPDF" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ]; then
            cd $OWTRAVIS_GITROOT
            if [ "$OWTRAVIS_DEBUG" = "1" ]; then pwd; fi
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
                git commit $GITQUIET -m "Travis CI build $TRAVIS_JOB_NUMBER (failure) - log files (OSX)"
            else
                git commit $GITQUIET -m "Travis CI build $TRAVIS_JOB_NUMBER (failure) - log files (Linux)"
            fi
            git push $GITQUIET -f origin
            cd $TRAVIS_BUILD_DIR
            if [ "$OWTRAVIS_DEBUG" = "1" ]; then pwd; fi
            echo_msg="gitupdf.sh - done"
        fi
    fi
#elif [ "$COVERITY_SCAN_BRANCH" = 1 ]; then
#else
fi

echo "$echo_msg"
