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
        GITQUIET=-v
    else
        GITQUIET=--quiet
    fi

    echo_msg="gitupds.sh - skipped"

    if [ "$TRAVIS_BRANCH" = "$OWBRANCH" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ] && [ "$TRAVIS_OS_NAME" = "linux" ]; then
            case "$OWTRAVISJOB" in
                "BOOTSTRAP")
                    ;;
                "BUILD" | "BUILD1" | "BUILD2" | "DOCPDF")
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
                    # copy OW build to git tree
                    #
                    export OWRELROOT=$OWTRAVIS_BUILD_DIR
                    cd $OWSRCDIR
                    case "$OWTRAVISJOB" in
                        "BUILD")
                            builder cprel
                            ;;
                        "BUILD1")
                            builder cprel1
                            ;;
                        "BUILD2")
                            builder cprel2
                            ;;
                        "DOCPDF")
                            builder cpdocpdf
                            ;;
                        *)
                            ;;
                    esac
                    #
                    # commit updated files to GitHub repository
                    #
                    cd $OWTRAVIS_BUILD_DIR
                    git add -f .
                    case "$OWTRAVISJOB" in
                        "BUILD")
                            git commit $GITQUIET -m "Travis CI build $TRAVIS_JOB_NUMBER - OW distribution"
                            ;;
                        "BUILD1")
                            git commit $GITQUIET -m "Travis CI build $TRAVIS_JOB_NUMBER - OW distribution 1"
                            ;;
                        "BUILD2")
                            git commit $GITQUIET -m "Travis CI build $TRAVIS_JOB_NUMBER - OW distribution 2"
                            ;;
                        "DOCPDF")
                            git commit $GITQUIET -m "Travis CI build $TRAVIS_JOB_NUMBER - Documentation"
                            ;;
                        *)
                            ;;
                    esac
                    git push $GITQUIET -f origin
                    cd $TRAVIS_BUILD_DIR
                    echo_msg="gitupds.sh - done"
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
