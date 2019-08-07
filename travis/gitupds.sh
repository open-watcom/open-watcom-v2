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

    if [ "$TRAVIS_EVENT_TYPE" = "push" ] || [ "$TRAVIS_EVENT_TYPE" = "cron" ]; then
        case "$TRAVIS_BUILD_STAGE_NAME" in
            "Update build" | "Update build windows")
                if [ "$TRAVIS_OS_NAME" = "linux" ] || [ "$TRAVIS_OS_NAME" = "windows" ]; then
                    #
                    # clone GitHub repository
                    #
                    git clone $GITVERBOSE1 --branch=master https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_BUILD_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
                    cd $OWTRAVIS_BUILD_DIR
                    depth=`git rev-list HEAD --count`
                    if [ $depth -gt 20 ]; then
                        echo "gitupds.sh - start compression"
                        git checkout --orphan temp1
                        git add $GITVERBOSE2 -A
                        git commit $GITVERBOSE1 -am "Initial commit"
                        git branch $GITVERBOSE1 -D master
                        git branch $GITVERBOSE1 -m master
                        git push $GITVERBOSE1 -f origin master
                        git branch $GITVERBOSE1 --set-upstream-to=origin/master master
                        echo "gitupds.sh - end compression"
                    fi
                    cd $TRAVIS_BUILD_DIR
                    #
                    # copy OW build to git tree
                    #
                    if [ "$TRAVIS_OS_NAME" = "linux" ]; then
                        cp -Rf $OWRELROOT/. $OWTRAVIS_BUILD_DIR/
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
            "Release" | "Release windows")
                ;;
            *)
                ;;
        esac
    fi

    echo "$echo_msg"

    return 0
}

gitupds_proc $*
