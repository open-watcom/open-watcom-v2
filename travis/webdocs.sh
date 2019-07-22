#!/bin/sh
#
# Script to build the Open Watcom tools on Travis
# using the host platform's native C/C++ compiler.
#
# Expects 
#   - POSIX tools
#   - correct setup for all OW build environment variables
#

build_proc()
{
    #
    # clone GitHub repository WEBDOCS
    #
    git clone $GITVERBOSE1 --branch=master https://${GITHUB_TOKEN}@github.com/${OWWEBDOCS_REPO_SLUG}.git $OWWEBDOCS_BUILD_DIR
    #
    # compress GitHub repository to hold only a few last builds
    #
    cd $OWWEBDOCS_BUILD_DIR
    depth=`git rev-list HEAD --count`
    if [ $depth -gt 20 ]; then
        echo "webdocs.sh - start compression"
        git checkout --orphan temp1
        git add $GITVERBOSE2 -A
        git commit $GITVERBOSE1 -am "Initial commit"
        git branch $GITVERBOSE1 -D master
        git branch $GITVERBOSE1 -m master
        git push $GITVERBOSE1 -f origin master
        git branch $GITVERBOSE1 --set-upstream-to=origin/master master
        echo "webdocs.sh - end compression"
    fi
    cd $TRAVIS_BUILD_DIR

    #
    # create bootstrap tools directory
    #
    mkdir $OWBINDIR
    #
    # build wmake for host system
    #
    cd $OWSRCDIR/wmake
    mkdir $OWOBJDIR
    cd $OWOBJDIR
    make -f ../posmake clean
    make -f ../posmake TARGETDEF=-D__LINUX__
    RC=$?
    #
    # build builder for host system
    #
    if [ $RC -eq 0 ]; then
        cd $OWSRCDIR/builder
        mkdir $OWOBJDIR
        cd $OWOBJDIR
        $OWBINDIR/wmake -f ../binmake clean
        $OWBINDIR/wmake -f ../binmake bootstrap=1 builder.exe
        RC=$?
    fi
    #
    # build all pre-requisity tools for documentation build
    #
    if [ $RC -eq 0 ]; then
        cd $OWSRCDIR/watcom
        builder boot
        RC=$?
    fi
    if [ $RC -eq 0 ]; then
        cd $OWSRCDIR/builder
        builder boot
        RC=$?
    fi
    if [ $RC -eq 0 ]; then
        cd $OWSRCDIR/whpcvt
        builder boot
        RC=$?
    fi
    if [ $RC -eq 0 ]; then
        cd $OWSRCDIR/bmp2eps
        builder boot
        RC=$?
    fi
    #
    # build documentation for Wiki
    #
    if [ $RC -eq 0 ]; then
        cd $OWSRCDIR
        builder -i webdocs -- -i
#        RC=$?
    fi
    cd $TRAVIS_BUILD_DIR
    return $RC
}

build_proc $*
