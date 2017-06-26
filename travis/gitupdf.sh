#!/bin/sh
# *****************************************************************
# gitupdf.sh - update git repository if build fails
# *****************************************************************
#
# after failure transfer log files back to GitHub repository
#

gitupdf_proc()
{
    if [ "$OWTRAVIS_DEBUG" = "1" ]; then
        GITQUIET=-v
    else
        GITQUIET=--quiet
    fi
    
    echo_msg="gitupdf.sh - skipped"
    
    if [ "$TRAVIS_BRANCH" = "$OWBRANCH" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ]; then
            if [ "$OWTRAVISJOB" = "BOOTSTRAP" ] || [ "$OWTRAVISJOB" = "BUILD" ] || [ "$OWTRAVISJOB" = "DOCPDF" ]; then
                #
                # setup client info
                #
                git config --global user.email "travis@travis-ci.org"
                git config --global user.name "Travis CI"
                git config --global push.default simple
                #
                # clone GitHub repository
                #
                git clone $GITQUIET --branch=master https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
                cd $OWTRAVIS_BUILD_DIR
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
                echo_msg="gitupdf.sh - done"
            fi
        fi
    fi
    
    echo "$echo_msg"

    return 0
}

gitupdf_proc $*
