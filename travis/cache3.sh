#!/bin/sh
#
# Script to fill Travis build cache2
#
set -x

copy_tree3()
{
    cd $OWSRCDIR/$1
    dir1=$OWROOT/buildx/$1
    for x in `find . -name "$2" -type f`; do
        dirn=$dir1/`dirname "$x"`
        if [ ! -d "$dirn" ]; then
            mkdir -p "$dirn" >>$OWBINDIR/cache3.log
        fi
        cp $CP_OPTS "$x" "$dir1/$x" >>$OWBINDIR/cache3.log
    done
}

if [ "$1" = "load" ]; then
    git clone --depth=1 --branch=master https://${GITHUB_TOKEN}@github.com/${OWTRAVIS_BUILD_REPO_SLUG}.git $OWTRAVIS_BUILD_DIR
    echo "load cache" >>$OWBINDIR/cache3.log
    cp $CP_OPTS $OWROOT/buildx/. $OWSRCDIR/ >>$OWBINDIR/cache3.log; true
fi
if [ "$1" = "save" ]; then
    echo "save cache3" >$OWBINDIR/cache3.log
    #
    copy_tree3  setupgui    "*setup.exe"
    #
    cd $TRAVIS_BUILD_DIR
fi
