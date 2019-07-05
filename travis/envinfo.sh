#!/bin/sh
# *****************************************************************
# envinfo.sh - display environment variables
# *****************************************************************
#

envinfo_proc()
{
    if [ "$OWTRAVIS_ENV_DEBUG" = "1" ]; then
#        env | sed -n -e '/^TRAVIS/p' -e '/^OW/p' -e '/^COVERITY/p' | sort
        env | sort
    fi
    
    return 0
}

envinfo_proc $*
