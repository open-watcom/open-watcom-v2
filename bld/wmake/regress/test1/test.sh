#!/bin/sh

function usage() {
    echo usage: $0 prgname errorfile
    exit
}

function print_header() {
    echo \# -----------------------------
    echo \#   Test $TEST
    echo \# -----------------------------
}

function do_check() {
    if [ "$?" == "0" ]; then
        echo \# Test $TEST successful
    else
        echo \#\# INLINE \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
	exit
    fi
}

if [ "$2" == "" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===========================
echo \# Multiple Dependents Test
echo \# ===========================

TEST=1
../cmds/create 30
rm -f err1.out
echo >err1.out
$1 -h -f maketst1 -l err1.out > tst1.out
diff -b tst1.out tst1u.chk
diff -b err1.out err1.chk
do_check

rm *.obj
rm *.out
rm main.*
rm foo*.c
rm maketst1
