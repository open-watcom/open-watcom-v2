#!/bin/sh
# *****************************************************************
# cacheinf.sh - display cache informations
# *****************************************************************
#

if [ "$OWTRAVIS_DEBUG" = "1" ]; then
    echo "****** CACHE INFO ******"
    echo "*** paths **************"
    cat $CASHER_DIR/paths
    echo "*** push.log ***********"
    cat $CASHER_DIR/push.log
    echo "*** push.err.log *******"
    cat $CASHER_DIR/push.err.log
    echo "*** fetch.log **********"
    cat $CASHER_DIR/fetch.log
    echo "*** fetch.err.log ******"
    cat $CASHER_DIR/fetch.err.log
    echo "*** tar.log ************"
    cat $CASHER_DIR/tar.log
    echo "*** tar.err.log ********"
    cat $CASHER_DIR/tar.err.log
    echo "************************"
fi
