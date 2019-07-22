#!/bin/sh

set +x

coverity_load_proc()
{
    PLATFORM=`uname`
    TOOL_ARCHIVE=/tmp/cov-analysis-${PLATFORM}.tgz
    TOOL_BASE=/tmp/coverity-scan-analysis

#    wget -nv -O $TOOL_ARCHIVE https://scan.coverity.com/download/cxx/linux64 --post-data "project=$TRAVIS_REPO_SLUG&token=$COVERITY_SCAN_TOKEN"
    curl -L https://entrust.com/root-certificates/entrust_l1k.cer -o l1k.crt
    curl --cacert l1k.crt -o $TOOL_ARCHIVE https://scan.coverity.com/download/cxx/linux64 -d "project=$TRAVIS_REPO_SLUG&token=$COVERITY_SCAN_TOKEN"

    mkdir -p $TOOL_BASE
    tar xzf $TOOL_ARCHIVE -C $TOOL_BASE
    
    TOOL_DIR=`find $TOOL_BASE -type d -name 'cov-analysis*'`
    export PATH=$TOOL_DIR/bin:$PATH
    return 0
}

coverity_run_proc()
{
    export BUILDER_OPT=-q
    cov-build --dir cov-int travis/covbuild.sh
    #cov-import-scm --dir cov-int --scm git --log cov-int/scm_log.txt 2>&1
    return 0
}

coverity_upload_proc()
{
    RESULTS_ARCHIVE=open-watcom-v2.tgz
    tar czf $RESULTS_ARCHIVE cov-int
    SHA=`git rev-parse --short HEAD`

    response=$(curl -v --cacert l1k.crt \
      --write-out "\n%{http_code}\n" \
      --form project=$TRAVIS_REPO_SLUG \
      --form token=$COVERITY_SCAN_TOKEN \
      --form email=malak.jiri@gmail.com \
      --form file=@$RESULTS_ARCHIVE \
      --form version=$SHA \
      --form description="Open Watcom V2 Travis CI build" \
      https://scan.coverity.com/builds)
    return 0
}

coverity_load_proc
coverity_run_proc
coverity_upload_proc
