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
        GITVERBOSE1=-v
        GITVERBOSE2=-v
    else
        GITVERBOSE1=--quiet
        GITVERBOSE2=
    fi

    echo_msg="gitupdf.sh - skipped"

    if [ "$TRAVIS_BRANCH" = "$OWBRANCH" ] || [ "$TRAVIS_BRANCH" = "$OWBRANCH_DOCS" ]; then
        if [ "$TRAVIS_EVENT_TYPE" = "push" ] || [ "$TRAVIS_EVENT_TYPE" = "cron" ]; then
            case "$OWTRAVISJOB" in
                "BOOTSTRAP" | "BUILD" | "BUILD-1" | "BUILD-2" | "BUILD-3" | "DOCS" | "INST" | "WEBDOCS")
                    #
                    # clone GitHub repository
                    #
                    git clone $GITVERBOSE1 --branch=$OWBRANCH https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
                    #
                    # copy build log files to git repository tree
                    #
                    if [ "$TRAVIS_OS_NAME" = "osx" ]; then
                        OWLOGDIR = $OWTRAVIS_BUILD_DIR/logs/osx
                    elif [ "$TRAVIS_OS_NAME" = "windows" ]; then
                        OWLOGDIR = $OWTRAVIS_BUILD_DIR/logs/windows
                    else
                        OWLOGDIR = $OWTRAVIS_BUILD_DIR/logs/linux
                    fi
                    test -d $OWLOGDIR || mkdir -p $OWLOGDIR
                    cp $OWBINDIR/*.log $OWLOGDIR
                    cp $OWDOCSDIR/*.log $OWLOGDIR
                    cp $OWDISTRDIR/*.log $OWLOGDIR
                    #
                    # commit new log files to GitHub repository
                    #
                    cd $OWTRAVIS_BUILD_DIR
                    git add $GITVERBOSE2 -f .
                    if [ "$TRAVIS_OS_NAME" = "osx" ]; then
                        git commit $GITVERBOSE1 -m "Travis CI build $TRAVIS_JOB_NUMBER (build failure) - log files (OSX)"
                    elif [ "$TRAVIS_OS_NAME" = "windows" ]; then
                        git commit $GITVERBOSE1 -m "Travis CI build $TRAVIS_JOB_NUMBER (build failure) - log files (Windows)"
                    else
                        git commit $GITVERBOSE1 -m "Travis CI build $TRAVIS_JOB_NUMBER (build failure) - log files (Linux)"
                    fi
                    git push $GITVERBOSE1 -f origin
                    cd $TRAVIS_BUILD_DIR
                    echo_msg="gitupdf.sh - done"
                    ;;
                "TEST")
                    #
                    # clone GitHub repository
                    #
                    git clone $GITVERBOSE1 --branch=$OWBRANCH https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
                    #
                    # copy build log files to git repository tree
                    #
                    case "$OWTRAVISTEST" in
                        "WASM")
                            test -d $OWTRAVIS_BUILD_DIR/logs/linux/wasmtest || mkdir -p $OWTRAVIS_BUILD_DIR/logs/linux/wasmtest
                            cp $OWSRCDIR/wasmtest/result.log $OWTRAVIS_BUILD_DIR/logs/linux/wasmtest/
                            cp $OWSRCDIR/wasmtest/test.log $OWTRAVIS_BUILD_DIR/logs/linux/wasmtest/
                            ;;
                        "C")
                            test -d $OWTRAVIS_BUILD_DIR/logs/linux/ctest || mkdir -p $OWTRAVIS_BUILD_DIR/logs/linux/ctest
                            cp $OWSRCDIR/ctest/result.log $OWTRAVIS_BUILD_DIR/logs/linux/ctest/
                            cp $OWSRCDIR/ctest/test.log $OWTRAVIS_BUILD_DIR/logs/linux/ctest/
                            ;;
                        "CXX")
                            test -d $OWTRAVIS_BUILD_DIR/logs/linux/plustest || mkdir -p $OWTRAVIS_BUILD_DIR/logs/linux/plustest
                            cp $OWSRCDIR/plustest/result.log $OWTRAVIS_BUILD_DIR/logs/linux/plustest/
                            cp $OWSRCDIR/plustest/test.log $OWTRAVIS_BUILD_DIR/logs/linux/plustest/
                            ;;
                        "F77")
                            test -d $OWTRAVIS_BUILD_DIR/logs/linux/f77test || mkdir -p $OWTRAVIS_BUILD_DIR/logs/linux/f77test
                            cp $OWSRCDIR/f77test/result.log $OWTRAVIS_BUILD_DIR/logs/linux/f77test/
                            cp $OWSRCDIR/f77test/test.log $OWTRAVIS_BUILD_DIR/logs/linux/f77test/
                            ;;
                        "CRTL")
                            test -d $OWTRAVIS_BUILD_DIR/logs/linux/clibtest || mkdir -p $OWTRAVIS_BUILD_DIR/logs/linux/clibtest
                            cp $OWSRCDIR/clibtest/result.log $OWTRAVIS_BUILD_DIR/logs/linux/clibtest/
                            cp $OWSRCDIR/clibtest/test.log $OWTRAVIS_BUILD_DIR/logs/linux/clibtest/
                            ;;
                        *)
                            ;;
                    esac
                    #
                    # commit new log files to GitHub repository
                    #
                    cd $OWTRAVIS_BUILD_DIR
                    git add $GITVERBOSE2 -f .
                    git commit $GITVERBOSE1 -m "Travis CI build $TRAVIS_JOB_NUMBER (test failure) - log files (Linux)"
                    git push $GITVERBOSE1 -f origin
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
