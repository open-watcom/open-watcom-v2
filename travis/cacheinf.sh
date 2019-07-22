#!/bin/sh
# *****************************************************************
# cacheinf.sh - display cache informations
# *****************************************************************
#

cacheinf_proc()
{
    if [ "$OWTRAVIS_CACHE_DEBUG" = "1" ]; then
        echo "****** CACHE INFO ******"
        echo "`date`"
        echo "************************"
        ls -l $CASHER_DIR
        echo "*** paths **************"
        if [ -e $CASHER_DIR/paths ]; then cat $CASHER_DIR/paths; fi
        echo "*** push.log ***********"
        if [ -e $CASHER_DIR/push.log ]; then cat $CASHER_DIR/push.log; fi
        echo "*** push.err.log *******"
        if [ -e $CASHER_DIR/push.err.log ]; then cat $CASHER_DIR/push.err.log; fi
        echo "*** fetch.log **********"
        if [ -e $CASHER_DIR/fetch.log ]; then cat $CASHER_DIR/fetch.log; fi
        echo "*** fetch.err.log ******"
        if [ -e $CASHER_DIR/fetch.err.log ]; then cat $CASHER_DIR/fetch.err.log; fi
        echo "*** tar.log ************"
        if [ -e $CASHER_DIR/tar.log ]; then cat $CASHER_DIR/tar.log; fi
        echo "*** tar.err.log ********"
        if [ -e $CASHER_DIR/tar.err.log ]; then cat $CASHER_DIR/tar.err.log; fi
        echo "************************"
    fi
    
    return 0
}

cacheinf_proc $*
