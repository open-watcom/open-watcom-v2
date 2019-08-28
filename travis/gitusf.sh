#!/bin/sh
# *****************************************************************
# gitusf.sh - update sourceforge branch to trigger update
# *****************************************************************

###################################################################
# 1. update GitHub repository sourceforge branch by master branch
#

gitusf_proc1()
{
    if [ "$OWTRAVIS_DEBUG" = "1" ]; then
        GITVERBOSE1=-v
        GITVERBOSE2=-v
    else
        GITVERBOSE1=--quiet
        GITVERBOSE2=
    fi

    # git connection setup
    export GIT_ASKPASS=$OWTRAVISDIR/askpass.sh
    export SSH_PASSWORD=${SF_TOKEN2}
    #
    git remote set-url --push origin https://${SF_TOKEN1}@git.code.sf.net/p/openwatcom/open-watcom-v2
    if [ "$OWTRAVIS_DEBUG" = "1" ]; then echo "** git checkout"; fi
    git checkout master
    if [ "$OWTRAVIS_DEBUG" = "1" ]; then echo "** git push"; fi
    git push $GITVERBOSE1
    #
    
    echo "gitusf.sh - done"
    
    return 0
}

gitusf_proc1 $*
