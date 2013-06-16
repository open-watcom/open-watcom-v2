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
        echo \#\# PRETEST \#\# >> $LOGFILE
        echo Error: Test $TEST unsuccessful!!! | tee -a $LOGFILE
        exit
    fi
}

if [ "$2" == "" ]; then 
    usage
fi

LOGFILE=$2

echo \# ===================================
echo \# Start DOPRE
echo \# ===================================

TEST=1
print_header
$1 -h -f pre01 pre01 -f pre02 pre02
do_check

TEST=2
print_header
rm -f tmp.out
$1 -h -f pre03 pre03 > tmp.out 2>&1
diff pre03.chk tmp.out
do_check

TEST=3
print_header
$1 -h -f pre04 pre04
do_check

TEST=4
print_header
$1 -h -f pre05 pre05
do_check

TEST=5
print_header
$1 -h -f pre06 pre06
do_check

TEST=6
print_header
rm -f tmp.out
$1 -h -f pre07 > tmp.out 2>&1
diff pre07.chk tmp.out
do_check

TEST=7
print_header
rm -f tmp.out
$1 -h -f pre08 > tmp.out 2>&1
diff pre08.chk tmp.out
do_check

rm -f tmp.out
