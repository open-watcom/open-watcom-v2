if [ ! -d ../travis-ci-ow-builds ]; then mkdir -p ../travis-ci-ow-builds; fi
cd ../travis-ci-ow-builds
export OWRELROOT=`pwd`
cd $TRAVIS_BUILD_DIR
