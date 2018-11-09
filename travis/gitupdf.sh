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
        set -x
        GITQUIET=-v
    else
        GITQUIET=--quiet
    fi

    echo_msg="gitupdf.sh - skipped"

    if [ "$TRAVIS_BRANCH" = "$OWBRANCH" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ]; then
            case "$OWTRAVISJOB" in
                "BOOTSTRAP")
                "BUILD")
                "BUILD1")
                "BUILD2")
                "DOCPDF")
                    if [ "$TRAVIS_OS_NAME" = "osx" ] && [ "$OWOSXBUILD" != "1" ]; then
                        return 0
                    fi
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
                    #
                    # copy build log files to git repository tree
                    #
                    if [ "$TRAVIS_OS_NAME" = "osx" ]; then
                        test -d $OWTRAVIS_BUILD_DIR/logs/osx || mkdir -p $OWTRAVIS_BUILD_DIR/logs/osx
                        cp $TRAVIS_BUILD_DIR/build/$OWOBJDIR/*.log $OWTRAVIS_BUILD_DIR/logs/osx/
                    else
                        test -d $OWTRAVIS_BUILD_DIR/logs/linux || mkdir -p $OWTRAVIS_BUILD_DIR/logs/linux
                        cp $TRAVIS_BUILD_DIR/build/$OWOBJDIR/*.log $OWTRAVIS_BUILD_DIR/logs/linux/
                    fi
                    #
                    # commit new log files to GitHub repository
                    #
                    cd $OWTRAVIS_BUILD_DIR
                    git add -f .
                    if [ "$TRAVIS_OS_NAME" = "osx" ]; then
                        git commit $GITQUIET -m "Travis CI build $TRAVIS_JOB_NUMBER (failure) - log files (OSX)"
                    else
                        git commit $GITQUIET -m "Travis CI build $TRAVIS_JOB_NUMBER (failure) - log files (Linux)"
                    fi
                    git push $GITQUIET -f origin
                    cd $TRAVIS_BUILD_DIR
                    echo_msg="gitupdf.sh - done"
                    ;;
                *)
                    ;;
            esac
        fi
    fi

    echo "$echo_msg"

    return 0
}

gitupdf_proc $*
