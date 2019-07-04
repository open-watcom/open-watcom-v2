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
        if [ "$TRAVIS_OS_NAME" = "linux" ]; then
            if [ "$TRAVIS_EVENT_TYPE" = "push" ] || [ "$TRAVIS_EVENT_TYPE" = "cron" ]; then
                case "$OWTRAVISJOB" in
                    "BOOTSTRAP")
                        ;;
                    "BUILD" | "BUILD-1" | "BUILD-2" | "BUILD-3" | "DOCTRAVIS")
                        #
                        # clone GitHub repository
                        #
                        git clone $GITVERBOSE1 --branch=$OWBRANCH https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
                        #
                        # copy OW build to git tree
                        #
                        export OWRELROOT=$OWTRAVIS_BUILD_DIR
                        cd $OWSRCDIR
                        case "$OWTRAVISJOB" in
                            "BUILD")
                                builder cprel
                                ;;
                            "BUILD-1")
                                builder cprel1
                                ;;
                            "BUILD-2")
                                builder cprel2
                                ;;
                            "BUILD-3")
                                builder cprel3
                                ;;
                            "DOCTRAVIS")
                                builder cpdoctrav
                                ;;
                            *)
                                ;;
                        esac
                        #
                        # commit updated files to GitHub repository
                        #
                        cd $OWTRAVIS_BUILD_DIR
                        git add $GITVERBOSE2 -f .
                        case "$OWTRAVISJOB" in
                            "BUILD")
                                git commit $GITVERBOSE1 -m "Travis CI build $TRAVIS_JOB_NUMBER - OW distribution"
                                ;;
                            "BUILD-1")
                                git commit $GITVERBOSE1 -m "Travis CI build $TRAVIS_JOB_NUMBER - OW distribution 1"
                                ;;
                            "BUILD-2")
                                git commit $GITVERBOSE1 -m "Travis CI build $TRAVIS_JOB_NUMBER - OW distribution 2"
                                ;;
                            "BUILD-3")
                                git commit $GITVERBOSE1 -m "Travis CI build $TRAVIS_JOB_NUMBER - OW distribution 3"
                                ;;
                            "DOCTRAVIS")
                                git commit $GITVERBOSE1 -m "Travis CI build $TRAVIS_JOB_NUMBER - Documentation"
                                ;;
                            *)
                                ;;
                        esac
                        git push $GITVERBOSE1 -f origin
                        cd $TRAVIS_BUILD_DIR
                        echo_msg="gitupds.sh - done"
                        ;;
                    *)
                        ;;
                esac
            fi
        fi
    fi

    echo "$echo_msg"

    return 0
}

gitupds_proc $*
