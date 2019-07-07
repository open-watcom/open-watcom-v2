#!/bin/sh
# *****************************************************************
# gitupds.sh - update git repository if build succeeds
# *****************************************************************
#
# If OW build succeeds then transfer OW build to GitHub repository
#

gitupds_proc()
{
    if [ "$OWTRAVIS_DEBUG" = "1" ]; then
        set -x
        GITVERBOSE1=-v
        GITVERBOSE2=-v
    else
        GITVERBOSE1=--quiet
        GITVERBOSE2=
    fi

    echo_msg="gitupds.sh - skipped"

    if [ "$TRAVIS_BRANCH" = "$OWBRANCH" ] || [ "$TRAVIS_BRANCH" = "$OWBRANCH_DOCS" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ] || [ "$TRAVIS_EVENT_TYPE" = "cron" ]; then
            case "$OWTRAVISJOB" in
                "CPREL")
                    if [ "$TRAVIS_OS_NAME" = "linux" ] || [ "$TRAVIS_OS_NAME" = "windows" ]; then
                        #
                        # clone GitHub repository
                        #
                        git clone $GITVERBOSE1 --branch=$OWBRANCH https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
                        #
                        # copy OW build to git tree
                        #
                        pwd
                        if [ "$TRAVIS_OS_NAME" = "linux" ]; then
                            cp -Rf $OWRELROOT/. $OWTRAVIS_BUILD_DIR/
                        elif [ "$OWTRAVIS_DEBUG" = "1" ]; then
                            cp -Rfv $OWRELROOT/binnt64/. $OWTRAVIS_BUILD_DIR/binnt64/
                        else
                            cp -Rf $OWRELROOT/binnt64/. $OWTRAVIS_BUILD_DIR/binnt64/
                        fi
                        #
                        # commit updated files to GitHub repository
                        #
                        cd $OWTRAVIS_BUILD_DIR
                        pwd
                        git add $GITVERBOSE2 -f .
                        if [ "$TRAVIS_OS_NAME" = "linux" ]; then
                            git commit $GITVERBOSE1 -m "Travis CI build $TRAVIS_JOB_NUMBER - OW distribution"
                        else
                            git commit $GITVERBOSE1 -m "Travis CI build $TRAVIS_JOB_NUMBER - OW distribution (Windows 64-bit only)"
                        fi
                        git push $GITVERBOSE1 -f origin
                        cd $TRAVIS_BUILD_DIR
                        pwd
                        echo_msg="gitupds.sh - done"
                    fi
                    ;;
                "DOCTRAVIS")
                    if [ "$TRAVIS_OS_NAME" = "linux" ]; then
                        #
                        # clone GitHub repository
                        #
                        git clone $GITVERBOSE1 --branch=$OWBRANCH https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
                        #
                        # copy OW build to git tree
                        #
                        export OWRELROOT=$OWTRAVIS_BUILD_DIR
                        cd $OWSRCDIR
                        builder cpdoctrav
                        #
                        # commit updated files to GitHub repository
                        #
                        cd $OWTRAVIS_BUILD_DIR
                        git add $GITVERBOSE2 -f .
                        git commit $GITVERBOSE1 -m "Travis CI build $TRAVIS_JOB_NUMBER - Documentation"
                        git push $GITVERBOSE1 -f origin
                        cd $TRAVIS_BUILD_DIR
                        echo_msg="gitupds.sh - done"
                    fi
                    ;;
                *)
                    ;;
            esac
        fi
    fi

    echo "$echo_msg"

    return 0
}

gitupds_proc $*
