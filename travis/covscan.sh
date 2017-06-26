#!/bin/sh

set +x

coverity_load_proc()
{
    PLATFORM=`uname`
    TOOL_ARCHIVE=/tmp/cov-analysis-${PLATFORM}.tgz
    TOOL_BASE=/tmp/coverity-scan-analysis

    wget -nv -O $TOOL_ARCHIVE https://scan.coverity.com/download/${PLATFORM} --post-data "project=$TRAVIS_REPO_SLUG&token=$COVERITY_SCAN_TOKEN"

    mkdir -p $TOOL_BASE
    tar xzf $TOOL_ARCHIVE -C $TOOL_BASE
    
    TOOL_DIR=`find $TOOL_BASE -type d -name 'cov-analysis*'`
    export PATH=$TOOL_DIR/bin:$PATH
    return 0
}

coverity_run_proc()
{
    export BUILDER_OPT=-q
    cov-build --dir cov-int build.sh
    #cov-import-scm --dir cov-int --scm git --log cov-int/scm_log.txt 2>&1
    return 0
}

coverity_upload_proc()
{
    return 0
}

coverity_load_proc
coverity_run_proc
coverity_upload_proc
